#include <fstream>
#include <iostream>
#include <stdexcept>

#include "headers/ant.h"
#include "headers/graph.h"
#include "headers/dijkstra.h"
#include "visualize.cpp"

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
    
    std::string departure, target;
    size_t weight;
    
    while (inputFile >> departure >> target >> weight)
    {
        if (!std::holds_alternative<Node*>(graph[departure])) graph.addNode(new Node(departure));
        if (!std::holds_alternative<Node*>(graph[target])) graph.addNode(new Node(target));
        
        graph.addEdge(take(graph[departure]), take(graph[target]), weight);
    }
    
    inputFile.close();
    
    // Graph testing
    /* std::cout << "[Graph testing]\n" << std::endl;
    
    Node* x = new Node("7");
    graph.addNode(x);
    x->addNeighbour(take(graph["7"]), 4);
    (*x).addNeighbour(take(graph["1"]), 5);
    
    Node* y = new Node("8");
    graph.addNode(y);
    y->addNeighbour(x, 1);
    (*y).addNeighbour(take(graph["3"]), 8);
    
    graph.show();
    
    try { take(graph["7"])->removeNeighbour(take(graph["1"])); }
    catch (const std::bad_variant_access& e) { std::cout << "exception: bad_variant_access!" << std::endl; }
    
    graph.show();
    
    graph.removeNode(take(graph["3"]));
    graph.removeEdge(x, take(graph["3"]));
    take(graph["1"])->addNeighbour(y, 6);
    
    graph.show(); */
    
    // Dijkstra
    std::cout << "[Dijkstra]\n" << std::endl;

    Dijkstra dijkstra(graph);
    Way way1 = dijkstra.shortestWay("0", "874");

    std::cout << "shortest path found by Dijkstra: ";
    for (Node* node : way1.nodes) std::cout << node->getName() << " ";
    std::cout << "\nlength: " << way1.length << '\n' << std::endl;
    std::cout << "----------------------------------------------------------------------\n" << std::endl;
    
    // Ants
    std::cout << "[Ants]\n" << std::endl;

    AntColony colony(graph, 1.0, 2.0, 0.1, 100.0, 10, 100);
    auto [way2, lengths] = colony.shortestWay("0", "874");

    std::ofstream file("of.txt");
    for (size_t i = 0; i < lengths.size(); ++i) file << i << " " << lengths[i] << std::endl;
    file.close();

    std::cout << "shortest path found by ACO: ";
    for (Node* node : way2.nodes) std::cout << node->getName() << " ";
    std::cout << "\nlength: " << way2.length << '\n' << std::endl;

    // Visualization
    std::map<std::pair<Node*, Node*>, double> pheromones = colony.getPheromoneLevels();
    visualize(graph, pheromones, way2);

    return 0;
}
