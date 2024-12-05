#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class endpoint
{
public:
    endpoint(std::string ipaddr, uint16_t port)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = inet_addr(ipaddr.c_str());
        addr_.sin_port = htons(port);
    }

    struct sockaddr_in addr_;
};
