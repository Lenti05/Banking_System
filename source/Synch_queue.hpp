//
//  Synch_queue.hpp
//  Banking_system
//
//  Created by Pietro Lenti on 23/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//


#ifndef Synch_queue_hpp
#define Synch_queue_hpp

#include "Message.hpp"
#include <stdio.h>

#include <condition_variable>
#include <list>
#include <mutex>
#include <string>

// This class contains the queue of incoming messages associated to each node. It must be synchronized because each node has
// a thread that waits for incoming messages and put them at the end of the queue and a thread that get messages from the end
// of the queue.
class Synch_queue {
public:
    Synch_queue(): ID(0) {}
    Synch_queue(int index): ID(index) {}
    void initialize(int index) { ID = index;}
    void put(const Message& val);
    void get(Message& val);
    int get_ID(){return  ID;}
    void print();
    void erase(){q.erase(q.begin(), q.end());};
private:
    int ID;
    std::mutex mtx;
    std::condition_variable cond;
    std::list<Message> q;
};
#endif /* Synch_queue_hpp */