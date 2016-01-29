//
//  Synch_queue.cpp
//  Banking_system_tcp
//
//  Created by Pietro Lenti on 04/12/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//


#include "Synch_queue.hpp"
#include<iostream>
#include <mutex>
#include <string>

void Synch_queue::put(const Message& val)
{
    std::lock_guard<std::mutex> lck(mtx);                  //lock_guard is used because this function just add messages to the tail
    q.push_back(val);                                      //of the queue without any condition.
    cond.notify_one();
}
void Synch_queue::get(Message& val)
{
    std::unique_lock<std::mutex> lck(mtx);                 //in this case unique_lock provides more flexibility than lock_guard
    cond.wait(lck,[this]{return !q.empty(); });
    val = q.front();                                       //returns a reference to the first element in the list container
    q.pop_front();                                         //removes the first element in the list container
}


void Synch_queue::print()
{
    for (typename std::list<Message>::iterator it=q.begin(); it != q.end(); it++)
        std::cout << " " << it->to_string() << "\n";
}