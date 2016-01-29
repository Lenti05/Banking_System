//
//  Handler.cpp
//  Banking_system_tcp
//
//  Created by Pietro Lenti on 04/12/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#include "Handler.hpp"
#include <boost/asio.hpp>
#include "Message.hpp"
#include "split.hpp"
#include "to.hpp"

using boost::asio::ip::tcp;

// Handler class that takes advantage of boost::asio libraries to handle connections.
void Handler::operator()()
{
    // tcp_connection contains the socket associated to the connected client and it is continously listening to incoming messages.
    // It is worthwhile to notice that incoming messages are read asynchronously. That is very useful because in this way one
    // server thread is enough to deal with many connections. If communication were synchronous, we needed to instantiate one
    // thread for each connected client, listening to its messages. When tcp_connection reads a message, it puts it at the beginning
    // of message queue. When the read message is of type "finished", tcp_writer closes the connection with the client that has sent that message.
    class tcp_connection
    :public std::enable_shared_from_this<tcp_connection>
    {
    public:
        tcp_connection(tcp::socket socket, Synch_queue* message_queue, Synch_cout* synch_cout, int index_)
        :socket_(std::move(socket)),
        message_queue_(message_queue),
        s_cout(synch_cout),
        index(index_)
        {}
        
        void start()
        {
            do_read_header();
        }
    private:
        
        void do_read_header()
        {
            auto self(shared_from_this());
            boost::asio::async_read(socket_,
                                    boost::asio::buffer(read_msg.data(), Message::header_length),
                                    [this, self](boost::system::error_code ec, std::size_t)
                                    {
                                        if (!ec && read_msg.decode_header())
                                        {
                                            do_read_body();
                                        }
                                        else{
                                            socket_.close();
                                        }
                                    });
        }
        
        void do_read_body()
        {
            auto self(shared_from_this());
            boost::asio::async_read(socket_,
                                    boost::asio::buffer (read_msg.body(), read_msg.body_length()),
                                    [this, self](boost::system::error_code ec, std::size_t)
                                    {
                                        if(!ec)
                                        {
                                            std::string msg(read_msg.body(), read_msg.body_length());
                                            //s_cout->write(msg);
                                            std::vector<std::string> tokens = split(msg);
                                            message_queue_->put(read_msg);
                                            if(tokens[0] == "finished")
                                            {
                                                //s_cout->write("Node " + to<>(index) + " received finished from " + tokens[1]);
                                                socket_.close();
                                            }
                                            else
                                            {
                                                do_read_header();
                                                //    std::cout << msg << "\n";
                                            }
                                        }
                                    });
        }
        
        tcp::socket socket_;
        Message read_msg;
        Synch_queue* message_queue_;
        Synch_cout* s_cout;
        int index;
    };
    
    
    //tcp_server waits for incoming connections and instantiates a tcp_connection object for each connected client. When the number of
    // connected clients equals num_connectins, it stops accepting new connections.
    class tcp_server
    {
    public:
        tcp_server(boost::asio::io_service& io_service,
                   const tcp::endpoint& endpoint, Synch_queue* message_queue, Synch_cout* sync_cout, int index_,
                   size_t num_connections_)
        :acceptor_(io_service, endpoint),
        socket_(io_service),
        message_queue_(message_queue),
        count_connections(0),
        s_cout(sync_cout),
        index(index_),
        num_connections(num_connections_)
        {
            do_accept();
        }
        
    private:
        void do_accept()
        {
            acceptor_.async_accept(socket_,
                                   [this](boost::system::error_code ec)
                                   {
                                       if(!ec){
                                           //s_cout->write("Node " + to<>(index) + ": Somebody connected");
                                           std::make_shared<tcp_connection>(std::move(socket_), message_queue_, s_cout, index)->start();
                                           count_connections++;
                                           if(count_connections < num_connections)
                                           {
                                               do_accept();
                                           }
                                       }
                                       else{
                                            //s_cout->write("No more connections");
                                       }
                                   });
        }
        tcp::acceptor acceptor_;
        tcp::socket socket_;
        size_t num_connections;
        int count_connections;
        int index;
        Synch_queue* message_queue_;
        Synch_cout* s_cout;
    };
    try
    {
        // We need to instantiate an asio io_service object to take advantage of the I/O functionalties offered by boot/asio library.
        boost::asio::io_service io_service;
        // endpoint objecto contains a list of TCP endpoints associated to this server.
        tcp::endpoint endpoint(tcp::v4(), std::atoi(port_.c_str()));
        // A tcp_sercer object is instantiated to deadl with incoming connections.
        tcp_server server(io_service, endpoint, message_queue, s_cout, index, n_connections);
        // Asio::io_service object funcionalities are associated to the thread that will run this object.
        io_service.run();
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}