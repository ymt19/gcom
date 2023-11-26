#include "multicast.h"

int main(void) {
    sender_socket_t *sock;
    char buf[256];
    ssize_t len;
    endpoint_t dest1;
    endpoint_t dest2;

    sock = sender_socket();

    sendto_multicast(sock, buf, len, )

    sender_close(sock);
}