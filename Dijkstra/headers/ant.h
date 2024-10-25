#ifndef ANT_H
#define ANT_H

#include "way.h"
#include "graph.h"
#include <random>

class AntColony
{
    Graph& graph;
    std::map<std::pair<Node*, Node*>, double> pheromones;

    double alpha;
    double beta;
    double evaporation_rate;
    double pheromone_intensity;

    size_t ant_count;
    size_t iterations;

    double probability(Node* current, Node* next);
    void updatePheromones(const Way& way);
public:
    AntColony(Graph& g, double a, double b, double evap_rate, double pher_intensity, size_t ants, size_t iters)
        : graph(g), alpha(a), beta(b), evaporation_rate(evap_rate), pheromone_intensity(pher_intensity), ant_count(ants), iterations(iters)
    {
        for (const auto& node : graph.getNodes())
            for (const auto& neighbour : node->getNeighbours())
                pheromones[{node, neighbour.first}] = 1.0; // начальные феромоны
    }

    std::pair<Way, std::vector<int>> shortestWay(char departure, char target);
};

#endif
