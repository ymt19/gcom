#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 32768

int main(void)
{
    int ret;
    int sd;
    struct sockaddr_in sv_addr;
    char buff[MAXSIZE];

    sd = socket(AF_INET, SOCK_STREAM, 0);

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = inet_addr("192.168.10.2");
    sv_addr.sin_port = htons(10000);
    ret = connect(sd, (struct sockaddr *)&sv_addr, sizeof(sv_addr));

    for (int i = 0; i < 100; i++) {
        memset(buff, '\0', MAXSIZE);
        snprintf(buff, MAXSIZE, "%d\n", i);
        ret = send(sd, buff, MAXSIZE, 0);
        printf("%d send:%d\n", i, ret);
    }
}