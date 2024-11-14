#include "../multicast.hpp"
#include <iostream>
#include <string>

#define SRC_ADDR "127.0.0.1"
#define SRC_PORT 10000

int main(int argc, char *argv[]) {
    uint16_t id = stoi(std::string(argv[1]));
    int len;
    char buf[256];
    multicast::Socket sock = multicast::Socket();

    sock.open(SRC_PORT + id);

    for (;;) {
        len = sock.recvfrom(buf);
        buf[len] = '\0';
        std::cout << "recvfrom: " << buf << std::endl;
    }
    sock.close();
}