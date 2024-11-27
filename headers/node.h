#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <limits>
#include <vector>
#include <queue>
#include <set>
#include <map>

class Node
{
    const std::string name;
    std::set<std::pair<Node*, size_t>> neighbours;
public:
    Node(const std::string& aname) : name(aname) {}

    const std::string& getName() const { return name; }
    const std::set<std::pair<Node*, size_t>>& getNeighbours() const { return neighbours; }

    void addNeighbour(Node* neighbour, size_t weight) { neighbours.insert(std::make_pair(neighbour, weight)); }
    void removeNeighbour(Node* neighbour);
    void clearNeighbours();
};

#endif
