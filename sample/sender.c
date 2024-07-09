#include "multicast.h"
#include <stdio.h>

int main(void) {
    sender_socket_t *sock;

    sock = sender_socket();

    send_multicast(sock, "abc", 3);

    sender_close(sock);
}