#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <set>
#include <map>

namespace multicast
{

class endpoint_list
{
public:
    void add_endpoint(int id, char *ipaddr, uint16_t port)
    {
        body_.insert();
    }
private:
    class endpoint
    {
    public:
        endpoint(char *ipaddr, uint16_t port)
        {
            addr_.sin_family = AF_INET;
            addr_.sin_addr.s_addr = inet_addr(ipaddr);
            addr_.sin_port = htons(port);
        }
    private:
        struct sockaddr_in addr_;
    };

    std::map<int, endpoint> body_{};
    std::set<int> group_{};
};

} // namespace multicast
