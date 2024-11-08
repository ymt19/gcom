#include "../multicast.hpp"
#include <iostream>

#define SRC_ADDR "127.0.0.1"
#define SRC_PORT 10000
#define DEST_ADDR "127.0.0.1"
#define DEST_PORT 10001

int main(void) {
    int a;
    char buf[256];
    multicast::Socket sock = multicast::Socket();

    sock.open(DEST_PORT);
    for (;;) {
        sock.recvfrom(buf);
        std::cout << buf << std::endl;
    }
    sock.close();
}