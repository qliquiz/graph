#ifndef way_h
#define way_h

#include "node.h"

struct Way
{
    std::vector<Node*> nodes;
    int length;
    Way() : length(std::numeric_limits<int>::max()) {}
};

#endif
