#include "../multicast.hpp"
#include <iostream>
#include <string>

#define SRC_ADDR "127.0.0.1"
#define SRC_PORT 10000
#define DEST_ADDR "127.0.0.1"
#define DEST_PORT 10001

int main(void) {
    std::string str;
    multicast::Socket sock = multicast::Socket();

    sock.open(SRC_PORT);

    for (;;)
    {
        std::cin >> str;
        sock.sendto(str.c_str(), str.size());
    }
    sock.close();
}