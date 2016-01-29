//
//  Synch_cout.cpp
//  Banking_system
//
//  Created by Pietro Lenti on 24/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//





#include "Synch_cout.hpp"



void Synch_cout::write(const std::string line)
{
    mtx.lock();
    std::cout << line << "\n";
    mtx.unlock();
}