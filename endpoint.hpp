#pragma once

#include <vector>

namespace multicast
{
class Endpoint
{
public:
    Endpoint(/* args */);
    ~Endpoint();
private:
    /* data */
};

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

} // namespace multicast
