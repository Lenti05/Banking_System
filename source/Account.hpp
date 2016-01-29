//
//  Account.hpp
//  Banking_system_tcp
//
//  Created by Pietro Lenti on 11/12/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#ifndef Account_hpp
#define Account_hpp

#include <stdio.h>
#include <string>
#include <vector>

typedef unsigned long int amount;

//class containing all the data related to a banking account.
class Account {
public:
    Account(): node_index(0), amount(0) {}
    Account(const std::string owner_name_, const std::string number, const int index,
            amount starting_amount):
    owner_name{owner_name_}, account_number{number}, node_index{index}, amount{starting_amount}
    {}
   /* Account(const Account& _account): owner_name{_account.owner_name}, account_number{_account.account_number},
    amount{_account.amount}
    {}
    */
    std::string& get_owner_name() { return owner_name;}
    std::string& get_account_number() { return account_number;}
    amount get_amount() {return amount;}
    int get_node_index() {return node_index;}
    void change_amount(amount variation) {amount+= variation;}
                       
private:
    std::string owner_name;
    std::string account_number;
    int node_index;
    amount amount;
};

















#endif /* Account_hpp */
