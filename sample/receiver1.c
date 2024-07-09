#include "multicast.h"

int main(void) {
    receiver_socket_t *sock;
    int port;

    sock = receiver_socket(port);

    receiver_close(sock);
}