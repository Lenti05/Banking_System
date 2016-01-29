//
//  Node.cpp
//  Banking_system
//
//  Created by Pietro Lenti on 23/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#include <thread>
#include "Node.hpp"
#include "split.hpp"


using std::vector;
using std::string;


// Edge objects are initialized after the construction of this node because they contain the necessary data to establish connections
// with the neighbors. In the TCP/IP protocol a client can establish a connection with a server (instantiating a "client socket) only
// after the server has instantiated its own socket ("server" socket).
void Node::initialize_edges(vector<Edge>& neighbors){
    if (!adj_edges.empty())
        adj_edges.erase(adj_edges.begin(), adj_edges.end());
    for(auto neighbor:neighbors){
    adj_edges.insert(std::move(std::pair<int,Edge>(neighbor.get_index(),neighbor)));
    }
}
// When a node has to transfer money, only information about the neighbor nodes within the path leading to the node that receives money is
// necessary.
vector<Edge> Node::get_sub_adj_edges(const vector<int>& indices)
{
    vector<Edge> sub_edges;
    if(adj_edges.empty())
        return sub_edges;
    for(auto index:indices)
        if(index != -1)
          sub_edges.push_back(adj_edges[index]);
    return sub_edges;
}

// This Node can be involved in two protocols: the AsynchBellmanFord protocol that calculates shortest paths and TransferAmount protocol
// that transfers money from an account to another one along the shortest path connecting the two accounts.
// Messages "update", "ack", "inform_source" , "calculate_shotest_paths" and "finished" are related to AsynchBellmanFord.
// On the other hand "transfer_amount" and "finished" are related to TransferAmount.
void Node::process_message(Message& msg, boost::asio::io_service& io_service_)
{
    std::string line = msg.to_string();
    vector<string> tokens = split(line);
    std::string message_type = tokens[0];
        if(message_type == "update")
            Node::answer_to_update(msg, io_service_);
   else if(message_type == "ack")
            Node::answer_to_ack(msg);
   else if(message_type == "calculate_shortest_paths")
            Node::answer_to_calculate_shortest_paths(io_service_);
   else if(message_type == "transfer_amount")
       Node::answer_to_transfer_amount(msg, io_service_);
   else if(message_type == "finished")
            Node::answer_to_finished(msg);
   else
            cout << "There is something wrong!\n";
}
// This message is composed of <transfer_amount> <sender_index> <to_index> <amount>.
// <transfer_amount> specifies the type of message.
// <sender_index> is the ID of the node that has sent the message.
// <to_index> is the ID of the node that will eventually receive the amount.
// <amount> is the money to transfer.
void Node::answer_to_transfer_amount(Message& msg, boost::asio::io_service& io_service_)
{
    std::string line = msg.to_string();
    vector<string> tokens = split(line);
    const int sender_index = to<int>(tokens[1]);
    const int to_index = to<int>(tokens[2]);
    amount amount = to<unsigned long int>(tokens[3]);
    const int parent_index = get_parent_index(to_index);
    tcp::resolver resolver(io_service_);
    vector<boost::asio::ip::tcp::resolver::iterator> iterators;
    // sender_index can be -1 if this message was sent by the main thread, in this case this node is the one
    // from which money is taken and transfered.
    if(sender_index == -1)
    {
       syn_cout->write("Node " + to<>(index) + " answering to transfer amount from the main thread");
       transfering_account = true;
       amount_to_transfer = amount;
    }
    // Otherwise this node instantiates a tcp_writer object to send messages (the "finished" message when the transaction is completed)
    // to the sender node.
    else
    {
       syn_cout->write("Node " + to<>(index) + " answering to transfer amount from node " + to<>(sender_index));
       Edge sender_edge = adj_edges.at(sender_index);
       writers.emplace_back(io_service_, resolver.resolve({sender_edge.get_ip_address(), sender_edge.get_tcp_port()}),
                                 syn_cout, index, sender_index);
    }
    // parent_index is the ID of the node following this node in the path leading to the node that eventually will receive the amount.
    // If it is -1 this node is the final receiver node. In this case, this node sends back a "finished' message to its sender node
    // to confirm that the transaction was successfully and terminates the algorithm.
    if(parent_index == -1)
    {
        io_service_.run();
        change_amount(amount);
        syn_cout->write("Node " + to<>(index) + " received a payment of " + to<>(amount) + "£");
        syn_cout->write("Node" + to<>(index) + " sending Finished on " + to<>(index) + "-" + to<>(sender_index));
        char line[Message::max_body_length + 1];
        std::strcpy(line, ("finished "  + to<>(index)).c_str());
        Message msg;
        msg.body_length(std::strlen(line));
        std::memcpy(msg.body(), line, msg.body_length());
        msg.encode_header();
        for(auto& writer:writers)
            writer.send(msg);
        finished = true;
    }
    // If this node is not the final receiver, this message is relayed to parent node.
    else{
      Edge parent_edge = adj_edges.at(parent_index);
      writers.emplace_back(io_service_, resolver.resolve({parent_edge.get_ip_address(), parent_edge.get_tcp_port()}),
                                 syn_cout, index, parent_index);
      io_service_.run();
      syn_cout->write("Node" + to<>(index) + " sending Transfer_amount on " + to<>(index) + "-" + to<>(parent_index));
      char line[Message::max_body_length + 1];
      std::strcpy(line, ("transfer_amount " + to<>(index) + " " + to<>(to_index) + " " + to<>(amount)).c_str());
      Message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      for(auto& writer:writers)
        if(writer.get_to_index() == parent_index)
          writer.send(msg);
    }
}
// This message is composed of <calculate_shortest_paths> and can be sent only by the main thread to the source node.
// The source node in the AsynchBellmanFord protocol is the destination node of all shortest paths to compute.
void Node::answer_to_calculate_shortest_paths(boost::asio::io_service& io_service_)
{
    // This node is the source node. It wakes up and sends "update" messages to all its neighbors.
    wakeup(io_service_);
    change_distance(index, 0);
    int w = 0;
    source_index = index;
    syn_cout->write("Source node " + to<>(index) + " calculating shortest path");
    for(auto& writer:writers){
        syn_cout->write("Node" + to<>(index) + "sending Update " + to<>(w) + " on " + to<>(index) + "-" + to<>(writer.get_to_index()));
        char line[Message::max_body_length + 1];
        std::strcpy(line, ("update " + to<>(source_index) + " " + to<>(index) + " " + to<>(w)).c_str());
        Message msg;
        msg.body_length(std::strlen(line));
        std::memcpy(msg.body(), line, msg.body_length());
        msg.encode_header();
        writer.send(msg);
    }
}

// The wakeup function is called to instantiate the tcp_writer objects that permit this node to communicate with its neighbors.
void Node::wakeup(boost::asio::io_service& io_service_)
{
    state = State::awake;
    tcp::resolver resolver(io_service_);
    vector<boost::asio::ip::tcp::resolver::iterator> iterators;
    for(std::map<int, Edge>::iterator it = adj_edges.begin(); it != adj_edges.end() ; it++ )
        writers.emplace_back(
        io_service_, resolver.resolve({it->second.get_ip_address(), it->second.get_tcp_port()}),
                                 syn_cout, index, it->first);
    io_service_.run();
}
// This message is composed of <update> <source_index> <sender_index> <w>
// <update> specifies the type of the message.
// <source_index> is the ID of node towards which the shortest paths are calculated (source node).
// <sender_index> is the ID of the node that has sent this message.
// <w> is the distance from the sender node to the source node along the current (that distance is continuosly updated) shortest path.
void Node::answer_to_update(Message& msg, boost::asio::io_service& io_service_)
{
    if(state == State::sleeping)
        wakeup(io_service_);
    std::string line = msg.to_string();
    vector<string> tokens = split(line);
    source_index = to<int>(tokens[1]);
    int sender_index = to<int>(tokens[2]);
    int w = to<int>(tokens[3]);
    syn_cout->write("Node " + to<>(index) + " answering to update from node " + to<>(sender_index) );
    // The first time this node receives a "update" message from a sender node it sends it back a "ack" message.
    // That is necessary to terminate the algorithm.
    // if the current distance from this node to source node is greater than the distance from sender node to source node plus the weight of the edge
    // connecting sender node to this node, then the former distance takes the value of the latter. Furthermore the parent index (the index of the node
    // following this node in the path to source node) is updated with the value of sender index.
    if(w + adj_edges[sender_index].get_weight() < get_distance(source_index))
    {
        Counter counter;
        counter.node_index = sender_index;
        counter.counter = adj_edges.size() - 1;
        counter_acks.push_back(counter);
        change_distance(source_index, w + adj_edges[sender_index].get_weight());
        int distance = get_distance(source_index);
        change_parent_index(source_index, sender_index);
        for(auto& writer:writers){
            if(writer.get_to_index() != sender_index ){
              syn_cout->write("Node" + to<>(index) + " sending Update " + to<>(distance) + " on " + to<>(index) + "-" + to<>(writer.get_to_index()));
        char line[Message::max_body_length + 1];
        std::strcpy(line, ("update " + to<>(source_index) + " " + to<>(index) + " " + to<>(distance)).c_str());
        Message msg;
        msg.body_length(std::strlen(line));
        std::memcpy(msg.body(), line, msg.body_length());
        msg.encode_header();
        writer.send(msg);
            }
        }
    }
    // if the message doesn't update the distance of this node, this node sends an ack message back to the sender index.
    else{
        syn_cout->write("Node" + to<>(index) + " sending Ack on " + to<>(index) + "-" + to<>(sender_index));
        char line[Message::max_body_length + 1];
        std::strcpy(line, ("ack "  + to<>(index)).c_str());
        Message msg;
        msg.body_length(std::strlen(line));
        std::memcpy(msg.body(), line, msg.body_length());
        msg.encode_header();
        for(auto& writer:writers)
            if(writer.get_to_index() == sender_index)
                writer.send(msg);
    }
}
// This message is composed of <ack> <sender_index>
// Each time a node i obtains a new distance estimate from a neighbor j and sends out corrections to all of its neighbors, it waits for corresponding
// from all those neighbors before sending an acknowledgment to j. Bookeeping is needed to keep the different sets of acknowledgements saparate.
// Vector counter_acks carries out that task.
void Node::answer_to_ack(Message& msg)
{
    std::string line = msg.to_string();
    vector<string> tokens = split(line);
    int sender_index = to<int>(tokens[1]);
    syn_cout->write("Node " + to<>(index) + " answering to ack from node " + to<>(sender_index) );
    if(index != source_index){
      bool found = false;
        for(vector<Counter>::iterator it = counter_acks.begin(); it < counter_acks.end(); it++)
      {
        if(found) break;
        if(sender_index != it->node_index)
        {
            it->counter--;
            found = true;
            if(it->counter == 0){
                syn_cout->write("Node" + to<>(index) + " sending Ack on " + to<>(index) + "-" + to<>(it->node_index));
                char line[Message::max_body_length + 1];
                std::strcpy(line, ("ack "  + to<>(index)).c_str());
                Message msg;
                msg.body_length(std::strlen(line));
                std::memcpy(msg.body(), line, msg.body_length());
                msg.encode_header();
                for(auto& writer:writers)
                   if(writer.get_to_index() == it->node_index)
                      writer.send(msg);
                counter_acks.erase(it);
            }
        }
       }
     }
  else
  {
        // When this node collects either "update" messages or "ack" messages from all its neighbors, its distance from source node is set to the final value.
        // When this happens, this node informs its parent node.
         counter_finishing++;
      if(counter_finishing == adj_edges.size())
      {
            for(auto& writer:writers){
              syn_cout->write("Node" + to<>(index)+ " sending Finished on " + to<>(index) + "-" + to<>(writer.get_to_index()));
              char line[Message::max_body_length + 1];
              std::strcpy(line, ("finished "  + to<>(index)).c_str());
              Message msg;
              msg.body_length(std::strlen(line));
              std::memcpy(msg.body(), line, msg.body_length());
              msg.encode_header();
                writer.send(msg);
        }
            interrupt = true;
        }
  }
}

void Node::answer_to_finished(Message& msg)
{
    std::string line = msg.to_string();
    vector<string> tokens = split(line);
    int sender_index = to<int>(tokens[1]);
    syn_cout->write("Node " + to<>(index) + " answering to finished from node " + to<>(sender_index) );
   // When this node receives this message for the first time, it relays it to all its neighbors.
   if(!finished)
   {
             for(auto& writer:writers){
              syn_cout->write("Node" + to<>(index) + " sending Finished on " + to<>(index) + "-" + to<>(writer.get_to_index()));
              char line[Message::max_body_length + 1];
              std::strcpy(line, ("finished "  + to<>(index)).c_str());
              Message msg;
              msg.body_length(std::strlen(line));
              std::memcpy(msg.body(), line, msg.body_length());
              msg.encode_header();
              writer.send(msg);
             }
       finished = true;
   }
       // When this node has collected "finished" messages from all its neighbors, it can terminate.
       counter_finished++;
       if(counter_finished == writers.size())
       {
        if(transfering_account)
          change_amount(amount_to_transfer * -1);
        else{
          int parent_index = get_parent_index(source_index);
          int distance = get_distance(source_index);
          if(source_index != -1){
              syn_cout->write("Node: " + to<>(index) + " has parent node from source " + to<>(source_index) + ": " + to<>(parent_index));
              syn_cout->write("Node: " + to<>(index) + " has distance from source " + to<>(source_index) + ": " + to<>(distance));
          }
        }
           interrupt = true;
       }
}


// The operator() function starts the handler threads (the thread handling all the incoming connections and messages, we can also call it
// server thread), gets messages from the head of the message queue and process them calling the correspondig function.
void Node::operator()(){
    handler.set_num_connections(adj_edges.size());
    thread t{handler};
    boost::asio::io_service io_service;
    while(!interrupt)
    {
        Message msg;
        message_queue->get(msg);
        process_message(msg, io_service);
    }
    syn_cout->write("Node " + to<>(index) + " has terminated");
    t.join();
    message_queue->erase();
    try{
    for(auto& writer:writers)
        writer.close();
    }
    catch(std::__1::system_error e){e.std::exception::what();};
    writers.erase(writers.begin(), writers.end());
}











