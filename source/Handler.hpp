//
//  Handler.hpp
//  Banking_system_tcp
//
//  Created by Pietro Lenti on 04/12/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#ifndef Handler_hpp
#define Handler_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include "Synch_queue.hpp"
#include "Synch_cout.hpp"


struct Handler {
public:
    Handler(const std::string& port, Synch_queue* msgs_queue, Synch_cout* synch_cout, int ID)
    : port_{port}, message_queue{msgs_queue},  s_cout{synch_cout}, index{ID}, n_connections{0}
    {}
    
    Handler(const Handler& handler)
    :port_{handler.port_}, message_queue{handler.message_queue},  s_cout{handler.s_cout},
    index{handler.index}, n_connections{handler.n_connections}
    {}
    void set_num_connections(size_t num_connections)
    { n_connections = num_connections;}
    void operator()();
    
private:
    std::string port_;                // TCP port.
    int index;                        // index of the associated node.
    Synch_queue* message_queue;       // Pointer to the message queue of the associated node.
    Synch_cout* s_cout;
    size_t n_connections;             // number of connections that the Handler is going to establish.
};

#endif