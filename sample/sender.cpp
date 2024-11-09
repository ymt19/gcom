#include "../multicast.hpp"
#include <iostream>
#include <string>

#define SRC_ADDR "127.0.0.1"
#define SRC_PORT 10000
#define DEST1_ADDR "127.0.0.1"
#define DEST1_PORT 10001
#define DEST2_ADDR "127.0.0.1"
#define DEST2_PORT 10002

int main(void) {
    std::string str;
    multicast::Socket sock = multicast::Socket();

    sock.open(SRC_PORT);

    sock.add_endpoint(1, (char *)DEST1_ADDR, DEST1_PORT, true);
    sock.add_endpoint(2, (char *)DEST2_ADDR, DEST2_PORT, true);

    for (;;)
    {
        std::cin >> str;
        sock.sendto(str.c_str(), str.size(), 0);
    }
    sock.close();
}