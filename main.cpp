#include <fstream>
#include <iostream>
#include <stdexcept>

#include "headers/ant.h"
#include "headers/graph.h"
#include "headers/dijkstra.h"

// like 'typedef' or 'using'
Node* take(const std::variant<Node*, std::monostate> v)
{
    if (std::holds_alternative<Node*>(v)) return std::get<Node*>(v);
    throw std::bad_variant_access();
}

int main(int argc, char* argv[])
{
    Graph graph;
    
    // Filling the graph
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
    
    // Graph testing
    std::cout << "[Graph testing]\n" << std::endl;
    
    Node* x = new Node('x');
    graph.addNode(x);
    x->addNeighbour(take(graph['x']), 4);
    (*x).addNeighbour(take(graph['a']), 5);
    
    Node* y = new Node('y');
    graph.addNode(y);
    y->addNeighbour(x, 1);
    (*y).addNeighbour(take(graph['b']), 8);
    
    graph.show();
    
    try { take(graph['b'])->removeNeighbour(take(graph['a'])); }
    catch (const std::bad_variant_access& e) { std::cout << "exception: bad_variant_access!" << std::endl; }
    
    graph.show();
    
    graph.removeNode(take(graph['b']));
    graph.removeEdge(x, take(graph['c']));
    take(graph['a'])->addNeighbour(y, 6);
    
    graph.show();
    
    // Dijkstra
    std::cout << "[Dijkstra]\n" << std::endl;

    Dijkstra dijkstra(graph);
    Way way1 = dijkstra.shortestWay('a', 'j');

    std::cout << "shortest path found by Dijkstra: ";
    for (Node* node : way1.nodes) std::cout << node->getName() << " ";
    std::cout << "\nlength: " << way1.length << '\n' << std::endl;
    std::cout << "----------------------------------------------------------------------\n" << std::endl;
    
    // Ants
    std::cout << "[Ants]\n" << std::endl;

    AntColony colony(graph, 1.0, 2.0, 0.1, 100.0, 10, 100);
    auto [way2, lengths] = colony.shortestWay('a', 'j');

    std::ofstream file("of.txt");
    for (size_t i = 0; i < lengths.size(); ++i) file << i << " " << lengths[i] << std::endl;
    file.close();

    std::cout << "shortest path found by ACO: ";
    for (Node* node : way2.nodes) std::cout << node->getName() << " ";
    std::cout << "\nlength: " << way2.length << '\n' << std::endl;
    std::cout << "----------------------------------------------------------------------\n" << std::endl;

    return 0;
}
