//
//  Node.hpp
//  Banking_system
//
//  Created by Pietro Lenti on 23/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#ifndef Node_hpp
#define Node_hpp

#include <stdio.h>

#include<map>
#include<string>
#include<vector>

#include"Edge.hpp"
#include"to.hpp"
#include"Synch_cout.hpp"
#include"Account.hpp"
#include"Shortest_paths.hpp"
#include"Handler.hpp"
#include"tcp_writer.hpp"

#define INFINITY 10000
using namespace std;

typedef unsigned long int amount;
typedef int node_index;
typedef int counter;
enum class State {sleeping, awake};

struct Counter {
    int node_index = -1;
    size_t counter = 0;
};


class Node {
public:
    //constructor
    Node(int id, Synch_queue* q, Synch_cout* s_cout,  int n_nodes, std::string& address, std::string& port,
         Account* account, Shortest_paths* paths):
         index(id),  message_queue(q), interrupt(false), syn_cout(s_cout),
         n(n_nodes), counter_finishing(0),counter_finished(0),source_index(-1),  counter_nodes(0), finished(false),
         ip_address(address), tcp_port(port), state{State::sleeping}, account_data{account}, shortest_paths_data{paths},
         transfering_account(false), amount_to_transfer(0), handler(port, q, s_cout, id)
         {}
    //copy constructor
    Node(const Node& _node): index{_node.index}, message_queue{_node.message_queue}, n{_node.n}, interrupt{_node.interrupt}, syn_cout{_node.syn_cout}, adj_edges{_node.adj_edges},  counter_finishing{_node.counter_finishing}, counter_finished{_node.counter_finished}, source_index{_node.source_index},  counter_acks{_node.counter_acks}, counter_nodes{_node.counter_nodes}, finished{_node.finished}, ip_address{_node.ip_address}, tcp_port{_node.tcp_port},  state{_node.state}, account_data{_node.account_data}, shortest_paths_data{_node.shortest_paths_data},transfering_account{_node.transfering_account}, amount_to_transfer{_node.amount_to_transfer}, handler{_node.handler}
        {}
    
    //destructor
    ~Node()
    {
    /*    syn_cout->write("Node " + to<>(index) + " is destroying");
        try
        {
        for(auto& writer:writers){
                 writer.close();
        }
        }
        catch(boost::system::system_error e) {syn_cout->write(e.what());}
        writers.erase(writers.begin(), writers.end());
     */
    }
    // The Edge objects are initialized after the construction of this node.
    void initialize_edges(vector<Edge>&);
    // Functions used to access and modify the values of the members of Account and Shortest_paths objects.
    std::string& get_owner_name() { return(account_data->get_owner_name());};
    std::string& get_account_number() { return(account_data->get_account_number());};
    amount get_amount() { return(account_data->get_amount());};
    int get_node_index(){ return(account_data->get_node_index());};
    void change_amount(amount variation) { account_data->change_amount(variation);};
    int get_parent_index(int source_index_) {return(shortest_paths_data->get_parent_index(source_index_));};
    int get_distance(int source_index_) {return(shortest_paths_data->get_distance(source_index_));};
    void change_parent_index(int source_index_, int new_parent_index)
       {return(shortest_paths_data->change_parent_index(source_index_, new_parent_index));};
    void change_distance(int source_index_, int new_distance)
       {return(shortest_paths_data->change_distance(source_index_, new_distance));};
    // Getting functions.
    vector<Edge> get_sub_adj_edges(const vector<int>&);
    const int get_index() { return index;}
    Synch_queue* get_queue_pt() { return message_queue;};
    Account* get_account_ptr() { return account_data;};
    const std::string get_ip_address() {return ip_address;};
    const std::string get_tcp_port() {return tcp_port;};
    const std::string get_address() { return ip_address;}
    const std::string get_port() { return tcp_port;}
    // Functions related to the asychBellmanFord and TransferAmount protocols.
    void process_message(Message&, boost::asio::io_service&);
    void answer_to_update(Message&, boost::asio::io_service&);
    void answer_to_ack(Message&);
    void answer_to_finished(Message&);
    void answer_to_transaction_completed(Message&);
    void answer_to_calculate_shortest_paths(boost::asio::io_service&);
    void answer_to_transfer_amount(Message&, boost::asio::io_service&);
    void wakeup(boost::asio::io_service&);
    bool paths_already_computed(const int source_index) {
        return shortest_paths_data->get_distance(source_index) == 0;
    }
    void operator()();
private:
    std::map<int, Edge> adj_edges;      // This map matches each neighbor with the related edge object.
    std::list<tcp_writer> writers;      // List containing the tcp_writer object necessary to send messages to neighbors.
    Handler handler;                    // Function class used to handle the incoming messages from neighbors
    std::vector<Counter> counter_acks;   // Variables used in the asynchronous distributed Bellman Ford.
    int index;
    int n;
    size_t counter_finishing;
    size_t counter_finished;
    bool finished;
    int counter_nodes;
    int source_index;
    State state;
    bool interrupt;                     // It is used to interrupt the thread associated to this node.
    bool transfering_account;           // It has the value true if this node is currently transfering money.
    amount amount_to_transfer;          // Current amount to transfer.
    std::string ip_address;             // Ip address of the node.
    std::string tcp_port;               // port TCP number to handle incoming communications.
    Synch_cout* syn_cout;               // object to write on stdout in order to avoid race conditions.
    Synch_queue* message_queue;         // Pointer to the message queue.
    Account* account_data;              // Pointer to Account object, which contains all the information about the account.
    Shortest_paths* shortest_paths_data;// Pointer to Shortest_pahts object, which contains all the info about the shortest paths
                                        // leading to any other node in the network.
};
#endif /* Node_hpp */