//
//  to.hpp
//  Banking_system
//
//  Created by Pietro Lenti on 23/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//
#include <string>
#include <sstream>
#ifndef to_hpp
#define to_hpp

//template function that just converts a type into another one.
template<typename Target=std::string, typename Source=std::string>  //string are used as default types for Target and Source
Target to(Source arg)
{
    std::stringstream interpreter;
    Target result;
    
    if(!(interpreter << arg)
       || !(interpreter >> result)
       || !(interpreter >> std::ws).eof())
        throw std::runtime_error{"to<>() failed"};
    
    return result;
}
#endif /* to_hpp */
