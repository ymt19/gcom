#include "tcp_connection_manager.hpp"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>

tcp_connection_manager::tcp_connection_manager(configuration *_config, std::queue<transaction> *_requests)
{
    config = _config;
    requests = _requests;
}

tcp_connection_manager::~tcp_connection_manager()
{
    if (worker.joinable())
    {
        worker.join();
    }
}

void tcp_connection_manager::run()
{
    std::cout << "run" << std::endl;

    if (config->id == 1)
    {
        worker = std::thread([this]{ sender(); });
    }
    else
    {
        worker = std::thread([this]{ receiver(); });
    }
}

void tcp_connection_manager::sender()
{
    int ret;
    struct sockaddr_in sv_addr;

    for (int slave = 0; slave < config->slaves; slave++)
    {
        connect_fd[slave] = socket(AF_INET, SOCK_STREAM, 0);
        if(connect_fd[slave] < 0) {
            fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
            exit(1);
        }

        sv_addr.sin_family = AF_INET;
        sv_addr.sin_addr.s_addr = inet_addr(config->slave_ipadder[slave]);
        sv_addr.sin_port = htons(config->slave_port[slave]);

        ret = connect(connect_fd[slave], (struct sockaddr *)&sv_addr, sizeof(sv_addr));
        if(ret != 0) {
            fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s\n", __LINE__, strerror(errno));
            exit(1);
        }
    }

    /****************** Senderプロトコル ******************/
    char buff[BUFFSIZE];
    int len;

    int seq = 1;
    while (1)
    {
        sleep(1);
        sprintf(buff, "from%d:seq%d", config->id, seq);
        seq++;
        for (int slave = 0; slave < config->slaves; slave++)
        {
            ret = send(connect_fd[slave], buff, strlen(buff), 0);
        }

        for (int slave = 0; slave < config->slaves; slave++)
        {
            len = recv(connect_fd[slave], buff, BUFFSIZE, 0);
            buff[len] = '\0';
            std::cout << buff << std::endl;
        }
    }
    /*****************************************************/
    
    for (int slave = 0; slave < config->slaves; slave++)
    {
        close(connect_fd[slave]);
    }
}

void tcp_connection_manager::receiver()
{
    int ret;
    struct sockaddr_in sv_addr;
    struct sockaddr_in cl_addr;
    int cl_addr_len;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd < 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = INADDR_ANY;
    sv_addr.sin_port = htons(config->port);

    ret = bind(listen_fd, (struct sockaddr *)&sv_addr, sizeof(sv_addr));
    if(ret != 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    ret = listen(listen_fd, SOMAXCONN);
    if(ret != 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }

    cl_addr_len = sizeof(cl_addr);
    connect_fd[0] = accept(listen_fd, (struct sockaddr *)&cl_addr, (socklen_t *)&cl_addr_len);
    if (connect_fd[0] < 0)
    {
        fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
        exit(1);
    }



    /****************** Recieverプロトコル ******************/
    char buff[BUFFSIZE];
    int len;

    while (1)
    {
        len = recv(connect_fd[0], buff, BUFFSIZE, 0);
        buff[len] = '\0';
        std::cout << buff << std::endl;
        sprintf(buff, "ackfrom%d", config->id);
        ret = send(connect_fd[0], buff, strlen(buff), 0);
    }
    /******************************************************/

    close(connect_fd[0]);
    close(listen_fd);
}