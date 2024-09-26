#pragma once

#include <vector>
#include "endpoint.hpp"

namespace multicast
{

class NeighborGroup
{
public:
    NeighborGroup(/* args */);
    ~NeighborGroup();
    void add_node();
private:
    std::vector<multicast::Endpoint> nodes;
};

class Tree
{
public:
    Tree(/* args */);
    ~Tree();
    void add_group();
private:
    std::vector<NeighborGroup> groups;
};

}
