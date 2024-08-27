#include "multicast.h"
#include <stdio.h>

int main(void) {
    sender_socket();

    send_multicast("abc", 3);

    send_multicast("abcd", 4);

    sender_close();
}