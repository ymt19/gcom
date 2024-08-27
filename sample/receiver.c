#include "multicast.h"
#include <stdio.h>

int main(void) {
    char buff[BUFSIZ];
    int len;
    endpoint_t endpoint;

    receiver_socket(10001);

    len = receive(buff, BUFSIZ, &endpoint);
    buff[len] = '\0';
    fprintf(stdout, "receive:%s\n", buff);

    receiver_close();
}