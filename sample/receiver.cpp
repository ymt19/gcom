#include "../multicast.hpp"
#include <iostream>
#include <string>

#define SRC_ADDR "127.0.0.1"
#define SRC_PORT 10000

// ポート番号 実行時間 ngnode1アドレス ngnode1ポート番号 ngnode2アドレス ngnode2ポート番号

int main(int argc, char *argv[]) {
    char buf[256];
    int len;
    uint16_t port;
    multicast::Socket sock{};

    port = stoi(std::string(argv[1]));

    sock.open(port);

    for (;;) {
        len = sock.recvfrom(buf);
        buf[len] = '\0';
        std::cout << "recvfrom: " << buf << std::endl;
    }
    sock.close();
}