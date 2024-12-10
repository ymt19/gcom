#include "../socket.hpp"
#include <iostream>
#include <string>
#include <vector>

// ポート番号 reciever1アドレス receiver1ポート番号 receiver2アドレス receiver2ポート番号

int main(int argc, char *argv[]) {
    char buf[256];
    int len;
    uint16_t port;
    int receivers;

    port = atoi(argv[1]);
    gcom::socket sock(port);

    // std::vector<gcom::endpoint> eps;
    gcom::endpoint ep(argv[2], atoi(argv[3]));

    // for (int i = 2; i < argc; i+=2)
    // {
    //     eps.push_back(gcom::endpoint(argv[i], atoi(argv[i+1])));
    // }

    for (;;)
    {
        std::cin >> buf;
        // sock.sendto(buf, strlen(buf), eps);
        sock.sendto(buf, strlen(buf), ep);
    }
}