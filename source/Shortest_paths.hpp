//
//  Shortest_paths.hpp
//  Banking_system_tcp
//
//  Created by Pietro Lenti on 11/12/2015.
//  Copyright © 2015 Pietro Lenti. All rights reserved.
//

#ifndef Shortest_paths_hpp
#define Shortest_paths_hpp

#define INFINITY 10000

#include <stdio.h>
#include <vector>
//class containing all the information related to shortest paths from the node associated to this class to any other nodes of the network.
class Shortest_paths {
public:
    Shortest_paths()
    : parent_indices(0, -1), distances(0, INFINITY), number_of_nodes{0}
    {}
    Shortest_paths(const int n)
    : parent_indices(n, -1), distances(n, INFINITY), number_of_nodes{n}
    {}
    Shortest_paths(const Shortest_paths& paths): number_of_nodes{paths.number_of_nodes},
    parent_indices(paths.parent_indices), distances(paths.distances)
    {}
    Shortest_paths& operator=(const Shortest_paths& paths)
    {
        number_of_nodes = paths.number_of_nodes;
        std::copy(paths.parent_indices.begin(), paths.parent_indices.end(), std::back_inserter(parent_indices));
        std::copy(paths.distances.begin(), paths.distances.end(), std::back_inserter(distances));
        return *this;
    }
    int get_parent_index(int source_index) {return parent_indices[source_index];}
    int  get_distance(int source_index) {return distances[source_index];}
    void change_parent_index(int source_index, int new_parent_index ) { parent_indices[source_index] = new_parent_index;}
    void change_distance(int source_index, int new_distance ) { distances[source_index] = new_distance;}
    
    
private:
    // vector indices represent the source indices, whereas the corresponding values are the parent indices in the AsynchBellmanFord protocol.
    std::vector<int> parent_indices;
    // vector indices represent the source indices, whereas the corresponding values are the distances in the AsynchBellmanFord protocol.
    std::vector<int> distances;
    // number of nodes of the network.
    int number_of_nodes;
};





#endif /* Shortest_paths_hpp */
