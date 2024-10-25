#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include "way.h"

class Dijkstra
{
    const Graph& graph;
public:
    Dijkstra(const Graph& agraph) : graph(agraph) {}
    Way shortestWay(char departure, char target);
};

#endif