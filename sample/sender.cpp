#include "../multicast.hpp"
#include <stdio.h>

#define SRC_ADDR "127.0.0.1"
#define SRC_PORT 10000
#define DEST_ADDR "127.0.0.1"
#define DEST_PORT 10001

int main(void) {
    char buf[256];
    int len;
    multicast::Socket sock = multicast::Socket();

    sock.open(SRC_PORT);
    sock.sendto(buf, len);
    sock.close();
}