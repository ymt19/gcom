#include "../multicast.hpp"
#include <iostream>
#include <string>

// ポート番号 n1アドレス n1ポート番号 n2アドレス n2ポート番号

int main(int argc, char *argv[]) {
    char buf[256];
    int len;
    uint16_t port;
    multicast::Socket sock{};

    port = atoi(argv[1]);

    sock.open(port);

    int id = 2;
    for (int i = 2; i < argc; i+=2)
    {
        sock.add_endpoint(id, argv[i], atoi(argv[i+1]), true);
        id++;
    }

    for (;;) {
        len = sock.recvfrom(buf);
        buf[len] = '\0';
        std::cout << "recvfrom: " << buf << std::endl;
    }
    sock.close();
}