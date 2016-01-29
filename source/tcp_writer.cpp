//
//  tcp_writer.cpp
//  Banking_system_tcp
//
//  Created by Pietro Lenti on 04/12/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#include "tcp_writer.hpp"
#include "to.hpp"

void tcp_writer::send(const Message& msg)
{
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if(!write_in_progress)
    {
        do_write();
        write_msgs_.pop_front();
    }
}


void tcp_writer::do_connect(tcp::resolver::iterator endpoint_iterator)
{
    boost::asio::async_connect(socket_, endpoint_iterator,
                               [this](boost::system::error_code ec, tcp::resolver::iterator)
                               {
                                 if(ec)
                                  s_cout->write("Node " + to<>(from_index) + " cannot establish a connection with node " + to<>(to_index));
                                  
                               });
}


void tcp_writer::do_write()
{
    boost::asio::async_write(socket_,
                             boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
                             [this](boost::system::error_code ec, std::size_t)
                             {
                                 if(!ec)
                                 {
                                     write_msgs_.pop_front();
                                     if (!write_msgs_.empty())
                                     {
                                         do_write();
                                     }
                                 }
                                 else
                                 {
                                     socket_.close();
                                 }
                             });
    
}








