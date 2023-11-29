#include <stdio.h>
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
    char buf[MAXSIZE];

    sd = socket(AF_INET, SOCK_STREAM, 0);

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = inet_addr(srv_config->srvs_ipaddr[target_id]);
    sv_addr.sin_port = htons(srv_config->srvs_port[target_id]);
    ret = connect(sd, (struct sockaddr *)&sv_addr, sizeof(sv_addr));

    ret = send(sd, buff, MAXSIZE, 0);
    printf("send:%d\n", ret);

    close(sd);
}