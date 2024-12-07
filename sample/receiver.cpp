#include "../socket.hpp"
#include <iostream>
#include <string>

// ポート番号 n1アドレス n1ポート番号 n2アドレス n2ポート番号

int main(int argc, char *argv[]) {
    char buf[256];
    int len;
    uint16_t port;

    port = atoi(argv[1]);

    gcom::socket sock(port);

    int id = 2;
    for (int i = 2; i < argc; i+=2)
    {
        gcom::endpoint ep(std::string(argv[i]), atoi(argv[i+1]));
        sock.add_group(ep);
    }

    gcom::endpoint src;
    for (;;) {
        len = sock.recvfrom(buf, src);
        buf[len] = '\0';
        std::cout << "recvfrom: " << buf << std::endl;
    }
}