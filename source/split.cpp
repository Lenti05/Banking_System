//
//  split.cpp
//  Banking_system
//
//  Created by Pietro Lenti on 23/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#include "split.hpp"

#include <algorithm>
#include <cctype>
#include <vector>
#include <string>

using std::find_if;
using std::string;
using std::vector;

bool space(char c)
{
    return isspace(c);
}

bool not_space(char c)
{
    return !isspace(c);
}

//Input: a string containg words separated by just one blank space.
//Output: a vector of strings representing the single words contained in the input string.
vector<string> split(const string& str)
{
    typedef string::const_iterator iter;
    vector<string> ret;
    
    iter i = str.begin();
    while ( i!= str.end()) {
        
        // ignore leading blanks
        i = find_if(i, str.end(), not_space);
        
        // find end of next word
        iter j = find_if(i, str.end(), space);
        
        if ( i != str.end())
            ret.push_back(string(i, j));
        i = j;
    }
    return ret;
}