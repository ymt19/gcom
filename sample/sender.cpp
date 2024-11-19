#include "../multicast.hpp"
#include <iostream>
#include <string>

// ポート番号 reciever1アドレス receiver1ポート番号 receiver2アドレス receiver2ポート番号

int main(int argc, char *argv[]) {
    char buf[256];
    int len;
    uint16_t port;
    multicast::Socket sock{};
    int receivers;

    port = atoi(argv[1]);

    sock.open(port);

    int id = 2;
    for (int i = 2; i < argc; i+=2)
    {
        sock.add_endpoint(id, argv[i], atoi(argv[i+1]), true);
        id++;
    }

    for (;;)
    {
        std::cin >> buf;
        sock.sendto(buf, strlen(buf), 0);
    }
    sock.close();
}