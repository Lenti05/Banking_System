//
//  main.cpp
//  Banking_system
//
//  Created by Pietro Lenti on 23/11/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//
// This is a simple banking accounts system that permits users to transfer money from one of their accounts to the desiderated account.
// Check file program_description.txt for an explanation of the program and algorithms used.



#include <iostream>
#include <vector>
#include <string>
#include <thread>


#include "Node.hpp"
#include "Message.hpp"
#include "Account.hpp"
#include "Shortest_paths.hpp"
#include "File_ptr.hpp"
#include "split.hpp"

using std::vector;
using std::string;


typedef std::string password;
typedef std::string user_name;
typedef std::string account_number;
typedef Synch_queue& account_queue_from;
typedef Synch_queue& account_queue_to;
typedef Synch_queue& account_queue_source;
typedef unsigned long int amount;

void calculate_shortest_paths( account_queue_source  , const int , vector<Node>& );
void transfer (account_queue_from, account_queue_to, amount,  const int, vector<Node>&);
void transfer_amount (account_queue_from, const int, const int, amount,  const int, vector<Node>&);
void display_accounts(std::list<Account*>& my_accounts);
const std::string current_date_time();


int main(int argc, const char * argv[]) {
    // In this piece of code all the necessary information are got from files
    // Read the passwords file
    std::string dir_path_passwords = ("../Test/");
    std::string file_passwords_name = (dir_path_passwords + "passwords.txt");
    //std::cout << "Enter the name of the password file" << "\n";
    //std::cin >> file_passwords_name;
    std::map<password, user_name> passwords;
    File_ptr f_reader(file_passwords_name, "r");
    char line[50];
    while(f_reader.read(line))
          {
              vector<std::string> tokens = split(line);
              std::string name = tokens[0] + " " + tokens[1];
              std::string password = tokens[2];
              passwords.insert(std::pair<std::string,std::string>(password,name));
          }
    
    // Read the account data file
    std::string dir_path_accounts_data = ("../Test/Input/Accounts_data/");
    std::string file_accounts_name = (dir_path_accounts_data + "data12.txt");
    std::vector<Node> vector_nodes;
    File_ptr fp(file_accounts_name, "r");
    char n_nodes[50];
    fp.read(n_nodes);
    int n = to<int>(n_nodes);
    vector<Synch_queue> queues_vector(n);
    vector<File_ptr*> log_file_writers(n);
    vector<Account> accounts_vector(n);
    vector<Shortest_paths> shortest_paths_vector(n);
    Synch_cout synch_cout;
    std::string address = "127.0.0.1";
    for(int i = 0; i < n; i++){
        char line[50];
        queues_vector[i].initialize(i);
        fp.read(line);
        vector<std::string> tokens = split(line);
        std::string owner_name = tokens[0] + " " + tokens[1];
        std::string account_number = tokens[2];
        amount amount = to<unsigned long int>(tokens[3]);
        accounts_vector[i] = Account(owner_name, account_number, i, amount);
        shortest_paths_vector[i] = Shortest_paths(n);
        std::string port = to<>(8765 + i);
        vector_nodes.push_back(Node(i, &queues_vector[i], &synch_cout, n, address, port,
                               &accounts_vector[i], &shortest_paths_vector[i]));
    }
    // Read the adjacency matrix file
    std::string dir_path_adj_matrix = ("../Test/Input/adj_matrices/");
    std::string file_adj_matrix = (dir_path_adj_matrix + "adj_matr12.txt");
    File_ptr matr_reader(file_adj_matrix, "r");
    for(int i=0; i< n; i++) {
        char line[300];
        matr_reader.read(line);
        vector<string> weights_vector = split(line);
        vector<Edge> edges_vector;
        for(int j = 0; j<n; j++) {
            if(to<int>(weights_vector[j]) != 0) {
                Edge edge{j, to<int>(weights_vector[j]), vector_nodes[j].get_queue_pt(), vector_nodes[j].get_address(),
                vector_nodes[j].get_port()};
                edges_vector.push_back(edge);
            }
        }
        vector_nodes[i].initialize_edges(edges_vector);
    }
    bool ok = false;
    password password_;
    user_name user_name_;
    std::string first_name;
    std::string family_name;
    
    // The user is asked to insert first name, family name and password to log in.
    while (!ok)
    {
    std::cout << "Insert your first name" << "\n";
    std::cin >>  first_name;
    std::cout << "Insert your family name" << "\n";
    std::cin >>  family_name;
    user_name_ = first_name + " " + family_name;
   
    std::cout << "Insert password" << "\n";
    std::cin >>  password_;
        try {
           if (passwords.at(password_) == user_name_)
               ok = true;
        } catch (std::exception e ) {
            std::cout << "invalid user name or password" << "\n";
        }
    }
    std::cout << "Welcome " + user_name_ +  "!" << "\n";
    std::vector<std::string> log_data;
    log_data.push_back("User name: " + user_name_);
    // The user can perform a transaction (a deposit) inserting the ID of the account from which
    // she wants to transfer money, the amount and the ID of the account receiving the money.
    bool do_that_again;
    do
    {
        // Before and after a transaction is performed all the data (account numbers and amounts) related to
        // the accounts the client owns are displayed.
        do_that_again = false;
    std::list<Account*> user_accounts;
    for(Node node:vector_nodes)
        if (node.get_owner_name() == user_name_)
            user_accounts.push_back(node.get_account_ptr());
    std::cout << "Your accounts:" << "\n";
    display_accounts(user_accounts);
    
    ok = false;
    account_number account_number_from;
    int index_from = -1;
    amount current_amount = 0;
    while (!ok)
    {
        // The account number inserted should match one of the accounts that the client owns.
        std::cout << "Insert the account number from which you want to transfer money" << "\n";
        std::cin >> account_number_from;
        for(Account* account_ptr:user_accounts)
            if(account_ptr->get_account_number() == account_number_from){
                ok = true;
                current_amount = account_ptr->get_amount();
                index_from = account_ptr->get_node_index();
            }
        if(!ok)
            std::cout << "Invalid account number" << "\n";
    }
    
    ok = false;
    amount amount_to_transfer = 0;
     while (!ok)
    {
        // The amount to transfer cannot overcome the account available credit.
        std::cout << "Insert the amount you want to transfer" << "\n";
        std::cin >> amount_to_transfer;
          if(current_amount > amount_to_transfer)
              ok = true;
        if(!ok)
            std::cout << "Credit insufficient" << "\n";
    }
    
    ok = false;
    account_number account_number_to;
    std::string receiver_first_name;
    std::string receiver_family_name;
    std::string receiver_name;
    int index_to = -1;
    while (!ok)
    {
        // The account that receives the money should match one of those present in the input account data file.
        std::cout << "Insert the first name of the receiver " << "\n";
        std::cin >> receiver_first_name;
        std::cout << "Insert the family name of the receiver " << "\n";
        std::cin >> receiver_family_name;
        receiver_name = receiver_first_name + " " + receiver_family_name;
        std::cout << "Insert the account number of the receiver" << "\n";
        std::cin >> account_number_to;
        for(Node node:vector_nodes)
            if(node.get_account_number() == account_number_to && node.get_owner_name() == receiver_name
                && account_number_from != account_number_to){
                ok = true;
                index_to = node.get_index();
            }
        if(!ok)
            std::cout << "Invalid account number" << "\n";
    }
    
    // The function "transfer" permits the main thread to communicate with the account threads (nodes).
    // That it is possible because in the main thread are stored the message queue objects of the single accounts threads,
    // whereas the nodes just have a pointer to their message queues.
    transfer(queues_vector[index_from], queues_vector[index_to], amount_to_transfer, n, vector_nodes);
    std::cout << "Your accounts:" << "\n";
    display_accounts(user_accounts);
    log_data.push_back("Sent " + to<>(amount_to_transfer) + "£ to " + receiver_name + " on account number " +
                       account_number_to + " on " +  current_date_time());
    std::string answer;
    std::cout << "Do you want to carry out another transaction? yes/no" << "\n";
    std::cin >> answer;
    if(answer == "yes")
        do_that_again = true;
    }while(do_that_again);

    //Generate output file

    std::string file_output_name = (family_name + "_" + current_date_time() + ".txt");
    std::string numbers_of_accounts = to<>(n_nodes);
    std::string dir_path_output = ("../Test/Output/adj_matrix12/");
    file_output_name = dir_path_output + file_output_name;
    File_ptr out_writer(file_output_name, "w");
    for(auto line:log_data)
        out_writer.write(line);
    out_writer.write(numbers_of_accounts);
    for (int j = 0; j < n; ++j)
    {
        std::string line = (accounts_vector[j].get_owner_name() + " " + accounts_vector[j].get_account_number()
                            + " " + to<>(accounts_vector[j].get_amount()) + "£");
        out_writer.write(line);
    }
    return 0;
}


void transfer(account_queue_from queue_from , account_queue_to queue_to, amount amount,
              const int nodes_number, vector<Node>& vector_nodes)
{
    const int from_index = queue_from.get_ID();
    const int to_index  = queue_to.get_ID();
    // The path joining the transfering account to the receiving account is computed via the asynchronous,
    // distributed version of The Bellman Ford algorithm. If this path is already been computed, it doesn't need
    // to run that algorithm again.
    if(!vector_nodes[to_index].paths_already_computed(to_index)){
        std::cout << "\n";
        calculate_shortest_paths(queue_to, nodes_number, vector_nodes);
        std::cout << "Shortest paths calculated" << "\n" << "\n";
    }
    transfer_amount(queue_from, from_index,  to_index, amount, nodes_number, vector_nodes);
}


void calculate_shortest_paths( account_queue_source message_queue, const int nodes_number, vector<Node>& vector_nodes)
{
    // The main thread sends a message to the receiving account in order to calculate all the shortest paths (one for each node)
    // leading to that account. The receiving account is identified as the source node in the asynchronous Bellman
    // Ford algorithm.
    vector<std::thread> t_nodes;
    for(int i=0; i< nodes_number; i++)
        t_nodes.push_back(std::thread{vector_nodes[i]});
    char line[Message::max_body_length + 1];
    std::strcpy(line, ("calculate_shortest_paths " ));
    Message msg;
    msg.body_length(std::strlen(line));
    std::memcpy(msg.body(), line, msg.body_length());
    msg.encode_header();
    message_queue.put(msg);
    for(int i=0; i< nodes_number; i++)
    {
        t_nodes[i].join();
    }
}

void transfer_amount(account_queue_from queue_from, const int from_index, const int to_index, amount amount, const int nodes_number, vector<Node>& vector_nodes)
{
    vector<std::thread> t_nodes;
    vector<Node> nodes;
    int previous_index = -1;
    int current_index = from_index;
    int following_index = vector_nodes[from_index].get_parent_index(to_index);
    int i = 0;
    int count_nodes = 0;
    // Unlike Bellman Ford algorithm (where all the nodes are involved), the algorithm to transfer money involves only the nodes
    // present in the path from the transfering node to the receiving node.
    do
    {
     vector<int> sub_indices = {previous_index, following_index};
     vector<Edge> sub_edges = vector_nodes[current_index].get_sub_adj_edges(sub_indices);
     nodes.push_back(vector_nodes[current_index]);
     nodes[i].initialize_edges(sub_edges);
     t_nodes.push_back(std::thread{nodes[i]});
     previous_index = current_index;
     current_index = following_index;
     following_index = vector_nodes[following_index].get_parent_index(to_index);
     count_nodes++;
     i++;
    }while ( current_index != to_index);
    vector<int> sub_indices = {previous_index, following_index};
    vector<Edge> sub_edges = vector_nodes[current_index].get_sub_adj_edges(sub_indices);
    nodes.push_back(vector_nodes[current_index]);
    nodes[i].initialize_edges(sub_edges);
    t_nodes.push_back(std::thread{nodes[i]});
    count_nodes++;
    char line[Message::max_body_length + 1];
    const int sender_index = -1;
    // The main thread sends a "transfer amount" message to the transfering account to start the algorithm.
    std::strcpy(line, ("transfer_amount " + to<>(sender_index) + " " + to<>(to_index) + " " + to<>(amount)).c_str());
    Message msg;
    msg.body_length(std::strlen(line));
    std::memcpy(msg.body(), line, msg.body_length());
    msg.encode_header();
    queue_from.put(msg);
    for(int i = 0 ; i< count_nodes ; i++)
    {
        t_nodes[i].join();
    }
    std::cout << "The transaction has been completed successfully" << "\n" << "\n";
}

void display_accounts(std::list<Account*>& user_accounts)
{
    // The main thread stores a list of pointers to the Account classes (which is the class storing all the data related to
    // a given account) that the user owns. Before and after a transaction is carried out all the updated data of the user accounts
    // are displayed.
    amount total_amount = 0;
    for(Account* account_ptr:user_accounts)
    {
        amount amount = account_ptr->get_amount();
        total_amount+= amount;
        std::cout << account_ptr->get_account_number() << " " << amount << "£" << "\n";
    }
    std::cout << "total amount = " << total_amount << "£" << "\n";
}


const std::string current_date_time() {
    time_t  now = time(0);
    struct tm tstruct;
    char   buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    
    return buf;
}


