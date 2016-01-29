//
//  tcp_writer.hpp
//  Banking_system_tcp
//
//  Created by Pietro Lenti on 04/12/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#ifndef tcp_writer_hpp
#define tcp_writer_hpp

#include <stdio.h>
#include <deque>
#include <boost/asio.hpp>
#include "Message.hpp"
#include "Synch_cout.hpp"
#include "to.hpp"

using boost::asio::ip::tcp;
typedef  std::deque<Message> message_queue;


//tcp_writer objects permits to send messages to their corresponding nodes.
class tcp_writer {
public:
    //Constructor
    tcp_writer(boost::asio::io_service& io_service,
               tcp::resolver::iterator endpoint_iterator, Synch_cout* synch_cout, int from_ID, int to_ID)
    :io_service_(io_service),
     socket_(io_service),
     s_cout(synch_cout),
     from_index(from_ID),
     to_index(to_ID)
    {
        //s_cout->write("Node " + to<>(from_index) + " created a tcp_writer object to send message to Node " + to<>(to_index));
        do_connect(endpoint_iterator);
    }
    //Copy constructor deleted
    tcp_writer(const tcp_writer& tcp_writer) = delete;
   
    // A tcp_writer object contains socket and io_service& that cannot be copied.
    //Move constructor
    tcp_writer(tcp_writer&& _tcp_writer): io_service_{_tcp_writer.io_service_}, socket_{std::move(_tcp_writer.socket_)},
    write_msgs_{_tcp_writer.write_msgs_}, s_cout{_tcp_writer.s_cout},
    from_index{_tcp_writer.from_index}, to_index{_tcp_writer.to_index}
    {
        _tcp_writer.write_msgs_.erase(_tcp_writer.write_msgs_.begin(), _tcp_writer.write_msgs_.end());
    }
    
    int get_from_index() {return from_index;}
    int get_to_index() {return to_index;}
    void send(const Message& msg);
    
    void close() {try{ socket_.close();}
        catch(const std::__1::system_error e){s_cout->write(e.what());}
    }
private:
    void do_connect(tcp::resolver::iterator endpoint_iterator);
    
    void do_write();
    
    
    
private:
    // The node thread instantiates a io_service object and passes a reference of it to each tcp_writer object it builds.
    boost::asio::io_service& io_service_;
    // Client socket.
    tcp::socket socket_;
    //Queue of sent messages. Object tcp_writer sends message asynchronously, so they need to be stored in write_msgs_
    //and then fetched by the server thread.
    message_queue write_msgs_;
    Synch_cout* s_cout;
    // Index of the node sending messages
    int from_index;
    // Index of the node receiving messages.
    int to_index;
};

#endif /* tcp_writer_hpp */
