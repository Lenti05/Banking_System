//
//  Synch_cout.hpp
//  Banking_system
//
//  Created by Pietro Lenti on 24/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//
#ifndef Synch_cout_hpp
#define Synch_cout_hpp

#include <stdio.h>
#include <iostream>
#include <mutex>
#include <string>
// class that permits to write on stdout without problems of race conditions.
class Synch_cout{
public:
    void write(const std::string);
private:
    std::mutex mtx;
};


#endif /* Synch_cout_hpp */