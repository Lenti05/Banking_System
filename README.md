# Banking_System

# Description
This is a simple banking accounts system that permits users to transfer money from one of their account to the desiderated account. An accurate description of the system is contained in the file Banking_System/source/program_description.docx.

# Input
Input files are contained in the folder Banking_System/bin/Test/Input and are divided into two folders: Accounts_data and adj_matrices. Folder adj_matrices contains the adjacency matrices representing the networks of banking accounts. Accounts_data, instead, contains all the information related to the banking accounts present in the system. For example, the file Accounts_data/data4.txt contains account owner name, account number and amount of all the accounts related to the network whose adjacenty matrix is represented by the file adj_matrices/adj_matr4.txt.  

# Output
Output files are contained within the folder Banking_System/bin/Test/Output and are classified according to the input adjacency matrix. Each files contains information related to a single session. They report: name of the user, the history of all transactions performed in the session and the updated information of all accounts present in the system (updated version of the input account data file).

# Dependencies

To execute this program is necessary to install the library boost/asio (http://www.boost.org/doc/libs/1_60_0/doc/html/boost_asio.html), for TCP sockets are used by nodes to communicate and C++ doesn't have standard libraries for networking.

# Contents (source folder)

Account: it is the class containing all the data related to a banking account. 

Edge: this class contains all the data associated to the edge connecting node i to node j.

File_ptr: RAII class for input and output files.

Handler: handler class used by Node class to handle connections and incoming messages from neighbor nodes.

Message: this class wraps the messages that nodes exchange.

Node: node class contains the protocols executed at a single node.

Shortest_paths: it contains all the information related to shortest paths from the node associated to this class to any other node of the network.

split: split function that divide into substrings an input string containing words separated by one blank space.

Synch_cout: class that permits to write on stdout without problems of race conditions.

Synch_queue: class that contains the queue of incoming messages associated to a node. Access to it is synchronized to avoid race conditions.

tcp_writer: each node must instantiate a tcp_writer object to communicate with a neighbor.

to: template function that just converts a type into another one.

# How to compile and run

I have executed and tested the program with Mac OS X using both CMake and Xcode. Here, for portability sake, I have included the cmakelists files necessary to compile the programs with CMake. To run the program with a different OS, you need just to provide slight modifications to file Banking_System/source/cmakelists.txt to take into account the difference in the file system.
To build the executable file, you should go to the directory where you want to store the file and type cmake followed by the path leading to the directory containing the source files. For building this program you have to go to the folder Banking_System/bin/banking_system and type:
 
"cmake path_to_folder_containing_the_project/Banking_System/source" 

This will generate the build files necessary for compilation. To compile the source files, type "make". Every time you edit  a source file you have to type "make" again to recompile the files. At this point, you need just to run the executable file to start the program.
To execute this program is necessary to install the library boost/asio (http://www.boost.org/doc/libs/1_60_0/doc/html/boost_asio.html)
to take advantage of its networking facilities (TCP sockets are used in the program). Once the library has been installed, you should indicate in the file Banking_System/source/cmakelists.txt the path to library "libboost_system.a" and to header files "boost/asio.hpp" (OSs different from Mac OS X have different files containing the library to link and the header files to include).

Running the program both with CMake and Xcode I noticed a marked difference in the number of threads that is possible to use before the 
OS starts complain about the high amount of open files in the system. With CMake this number is 12 threads, whereas with Xcode it seems to be much higher for I tested the program with 60 threads (nodes) and still it worked smoothly.





















