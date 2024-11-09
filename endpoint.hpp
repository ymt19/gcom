#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace multicast
{

class endpoint
{
public:
    endpoint(char *ipaddr, uint16_t port, bool is_group_node)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = inet_addr(ipaddr);
        addr_.sin_port = htons(port);
        is_group_node_ = is_group_node;
    }

    struct sockaddr_in addr_;
    bool is_group_node_;
};

} // namespace multicast
