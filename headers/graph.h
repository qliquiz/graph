#ifndef GRAPH_H
#define GRAPH_H

#include "node.h"

class Graph
{
    std::set<Node*> nodes;
public:
    ~Graph();

    void addNode(Node* node) { nodes.insert(node); }
    void removeNode(Node* node);
    void addEdge(Node* begin, Node* end, size_t weight) { begin->addNeighbour(end, weight); }
    void removeEdge(Node* begin, Node* end) { begin->removeNeighbour(end); }
    void show() const;

    const std::set<Node*>& getNodes() const { return nodes; }
    
    std::variant<Node*, std::monostate> operator[](const std::string l) const;
};

#endif
