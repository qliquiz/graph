#include <iostream>
#include <fstream>
#include <stdexcept>

#include "headers/graph.h"
#include "headers/dijkstra.h"

Node* take(const std::variant<Node*, std::monostate> v) {
    if (std::holds_alternative<Node*>(v)) return std::get<Node*>(v);
    throw std::bad_variant_access();
}

int main(int argc, char* argv[])
{
    Graph graph;
    
    
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open())
    {
        std::cerr << "can't open the file!" << std::endl;
        
        return -1;
    }
    
    char departure, target;
    size_t weight;
    
    while (inputFile >> departure >> target >> weight)
    {
        if (!std::holds_alternative<Node*>(graph[departure])) graph.addNode(new Node(departure));
        if (!std::holds_alternative<Node*>(graph[target])) graph.addNode(new Node(target));
        
        graph.addEdge(take(graph[departure]), take(graph[target]), weight);
    }
    
    inputFile.close();
    
    
    Node* d = new Node('d');
    graph.addNode(d);
    d->addNeighbour(take(graph['c']), 4);
    (*d).addNeighbour(take(graph['a']), 5);
    
    Node* e = new Node('e');
    graph.addNode(e);
    e->addNeighbour(d, 1);
    (*e).addNeighbour(take(graph['b']), 8);
    
    graph.show();
    
    try { take(graph['b'])->removeNeighbour(take(graph['a'])); }
    catch (const std::bad_variant_access& e) { std::cout << "exception: bad_variant_access!" << std::endl; }
    
    graph.show();
    
    graph.removeNode(take(graph['b']));
    graph.removeEdge(d, take(graph['c']));
    take(graph['a'])->addNeighbour(e, 6);
    
    graph.show();
    
    
    Dijkstra dijkstra(graph);
    Way way = dijkstra.shortestWay('c', 'd');

    std::cout << "shortest path: ";
    for (Node* node : way.nodes) std::cout << node->getName() << " ";
    std::cout << "\nlength: " << way.length << '\n' << std::endl;
    
    return 0;
}
