#include "../headers/node.h"

void Node::removeNeighbour(Node* neighbour)
{
    for (auto it = neighbours.begin(); it != neighbours.end(); ++it)
    {
        if (it->first == neighbour)
        {
            neighbours.erase(it);
            std::cout << "removed neighbour " << neighbour->getName() << " from node " << this->name << '\n' << std::endl;
            return;
        }
    }
}

void Node::clearNeighbours()
{
    neighbours.clear();
    std::cout << "neighbours of node " << this->name << " are cleared" << '\n' << std::endl;
}
