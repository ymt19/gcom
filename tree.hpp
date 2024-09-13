#pragma once

#include <vector>

namespace multicast
{

class NeighborGroup
{
public:
    NeighborGroup(/* args */);
    ~NeighborGroup();
    void add_node();
private:
    // std::vector<boost::asio::ip::udp::endpoint> nodes;
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
