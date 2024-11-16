#include "../multicast.hpp"
#include <iostream>
#include <string>

#define SRC_ADDR "127.0.0.1"
#define SRC_PORT 10000
#define DEST1_ADDR "127.0.0.1"
#define DEST1_PORT 10001
#define DEST2_ADDR "127.0.0.1"
#define DEST2_PORT 10002

// ポート番号 実行時間 reciever1アドレス receiver1ポート番号 receiver2アドレス receiver2ポート番号

int main(int argc, char *argv[]) {
    char buf[256];
    int len;
    uint16_t port;
    multicast::Socket sock{};

    port = stoi(std::string(argv[1]));

    sock.open(port);
    sock.add_endpoint(1, (char *)DEST1_ADDR, DEST1_PORT, true);
    sock.add_endpoint(2, (char *)DEST2_ADDR, DEST2_PORT, true);

    for (;;)
    {
        std::cin >> buf;
        sock.sendto(buf, strlen(buf), 0);
    }
    sock.close();
}