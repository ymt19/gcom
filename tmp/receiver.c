#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 32768

int main(void)
{
    int ret;
    int listen_sd, connection_sd;
    struct sockaddr_in sv_addr;
    struct sockaddr_in cl_addr;
    int cl_addr_len;
    char buff[MAX_SEND_DATA_SIZE];
    int msg_len;

    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sd < 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    sv_addr.sin_port = htons(port);

    ret = bind(listen_sd, (struct sockaddr *)&sv_addr, sizeof(sv_addr));
    if(ret != 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    ret = listen(listen_sd, SOMAXCONN);
    if(ret != 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    cl_addr_len = sizeof(cl_addr);
    connection_sd = accept(listen_sd, (struct sockaddr *)&cl_addr, (socklen_t *)&cl_addr_len);
    if (connection_sd < 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    memset(buff, -1, MAXSIZE);
    msg_len = recv(connection_sd, buff, MAX_SEND_DATA_SIZE, 0);
    printf("recv:%d\n", msg_len);
}