#include "multicast.h"
#include <stdio.h>

int main(void) {
    char buff[BUFSIZ];
    endpoint_t endpoint;

    receiver_socket(10001);

    receive(buff, BUFSIZ, &endpoint);

    receiver_close();

    fprintf(stderr, "EXIT_SUCCESS\n");
}