#include "multicast.h"
#include <stdio.h>

int main(void) {
    sender_socket();
    fprintf(stdout, "LINE:%d\n", __LINE__);

    send_multicast("abc", 3);
    fprintf(stdout, "LINE:%d\n", __LINE__);

    send_multicast("abcd", 4);
    fprintf(stdout, "LINE:%d\n", __LINE__);

    sender_close();
    fprintf(stdout, "LINE:%d\n", __LINE__);

    fprintf(stderr, "EXIT_SUCCESS\n");
}