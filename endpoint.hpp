#pragma once

#include <string>

namespace gcom
{

class endpoint
{
public:
    endpoint(std::string ipaddr, uint16_t port) : ipaddr(ipaddr), port(port) {}
    endpoint(const endpoint& ep) : ipaddr(ep.ipaddr), port(ep.port) {}

    std::string ipaddr;
    uint16_t port;
};

} // namespace gcom
