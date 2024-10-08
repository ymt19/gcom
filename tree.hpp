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

class MulticastGroup
{
public:
    MulticastGroup(/* args */);
    ~MulticastGroup();
    void add_node();
private:
    std::vector<NeighborGroup> groups;
};

}
