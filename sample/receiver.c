#include "multicast.h"
#include <stdio.h>

int main(void) {
    receiver_socket_t *sock;
    char buff[16];
    endpoint_t endpoint;

    sock = receiver_socket(10001);

    receive(sock, buff, 16, &endpoint);

    receiver_close(sock);

    fprintf(stderr, "EXIT_SUCCESS\n");
}