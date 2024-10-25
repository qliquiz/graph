#include "../headers/graph.h"

void Graph::removeNode(Node* node)
{
    for (auto it = nodes.begin(); it != nodes.end(); ++it) (*it)->removeNeighbour(node);
    
    if (nodes.find(node) != nodes.end()) {
        nodes.erase(node);
        std::cout << "removed node " << node->getName() << '\n' << std::endl;
        delete node;
    }
}

std::variant<Node*, std::monostate> Graph::operator[](const char l) const
{
    for (auto it = nodes.begin(); it != nodes.end(); ++it) if ((*it)->getName() == l) return *it;
    
    return std::monostate{};
}

Graph::~Graph()
{
    for (auto it = nodes.begin(); it != nodes.end(); ++it)
    {
        (*it)->clearNeighbours();
        delete *it;
    }
    
    nodes.clear();
    std::cout << "the graph is destroyed" << std::endl;
}

void Graph::show() const
{
    std::cout << "Graph:\n" << std::endl;

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        std::cout << (*it)->getName() << '\t';

        for (auto neighIt = (*it)->getNeighbours().begin(); neighIt != (*it)->getNeighbours().end(); ++neighIt)
            std::cout << neighIt->first->getName() << '-' << neighIt->second << ' ';
        
        std::cout << '\n' << std::endl;
    }
    
    std::cout << "----------------------------------------------------------------------\n" << std::endl;
}
