//
//  Edge.hpp
//  Banking_system
//
//  Created by Pietro Lenti on 23/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//
#ifndef Edge_hpp
#define Edge_hpp

#include <stdio.h>
#include <string>

#include "Synch_queue.hpp"
#include "tcp_writer.hpp"

class Edge {
public:
    //Default constructor
    Edge() {}
    //Constructor
    Edge( int ID, int w, Synch_queue* q, std::string address, std::string port):index(ID), weight(w), mq(q),
    ip_address(address), tcp_port(port)
    {}
    //Copy constructor
    Edge(const Edge& edge): index{edge.index},  weight{edge.weight}, mq{edge.mq}, ip_address{edge.ip_address},
    tcp_port{edge.tcp_port}
    {}
    //Move constructor
    Edge(Edge&& edge): index{edge.index},  weight{edge.weight}, mq{edge.mq}, ip_address{edge.ip_address},
    tcp_port{edge.tcp_port}
    {
        //edge.mq = nullptr;
    }
    const int get_weight() { return weight; }
    const int get_index() { return index; }
    const std::string get_ip_address() { return ip_address; }
    const std::string get_tcp_port() { return tcp_port; }
    
    
    
private:
    int index;                         // Index of node to which this edge leads.
    int weight;                        // Weight associated to this edge.
    std::string ip_address;            // IP address of the handler thread of node to which this edge leads.
    std::string tcp_port;              // TCP port of the handler thread of node to which this edge leads.
    Synch_queue* mq;                   // Message queue associated to this edge.
};
#endif /* Edge_hpp */