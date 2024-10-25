#ifndef WAY_H
#define WAY_H

#include "node.h"

struct Way
{
    std::vector<Node*> nodes;
    int length;
    Way() : length(std::numeric_limits<int>::max()) {}
};

#endif
