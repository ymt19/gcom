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
#include <sstream>
#include <cereal/archives/binary.hpp>

tcp_connection_manager::tcp_connection_manager(configuration& config, txqueue& requests, logger& lg) 
    : config(config), requests(requests), lg(lg), flag(ATOMIC_FLAG_INIT)
{
    std::cout << "run" << std::endl;

    if (config.id == 1)
    {
        worker = std::thread([this]{ sender(); });
    }
    else
    {
        worker = std::thread([this]{ receiver(); });
    }
}

tcp_connection_manager::~tcp_connection_manager()
{
    flag.test_and_set();

    // join
    if (worker.joinable())
    {
        worker.join();
    }
}

void tcp_connection_manager::sender()
{
    int ret;
    struct sockaddr_in sv_addr;

    for (int slave = 0; slave < config.slaves; slave++)
    {
        connect_fd[slave] = socket(AF_INET, SOCK_STREAM, 0);
        if(connect_fd[slave] < 0) {
            fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s", __LINE__, strerror(errno));
            exit(1);
        }

        sv_addr.sin_family = AF_INET;
        sv_addr.sin_addr.s_addr = inet_addr(config.slave_ipadder[slave]);
        sv_addr.sin_port = htons(config.slave_port[slave]);

        ret = connect(connect_fd[slave], (struct sockaddr *)&sv_addr, sizeof(sv_addr));
        if(ret != 0) {
            fprintf(stderr, "tcp_connection_manager.c: (line:%d) %s\n", __LINE__, strerror(errno));
            exit(1);
        }
    }

    /****************** Senderプロトコル ******************/
    char buff[BUFFSIZE];
    int len;

    while (!flag.test())
    {
        requests.mtx.lock(); /** lock **/
        if (!requests.data.empty())
        {
            std::stringstream ss;
            {
                cereal::BinaryOutputArchive oarchive(ss);
                oarchive(requests.data.front());
            }
            requests.data.pop();
            requests.mtx.unlock(); /** unlock **/

            len = ss.str().size();
            memcpy(buff, ss.str().c_str(), len);

            for (int slave = 0; slave < config.slaves; slave++)
            {
                ret = send(connect_fd[slave], buff, len, 0);
                // lg.send_message(ret, std::string(config.slave_ipadder[slave]), config.slave_port[slave]);
            }

            for (int slave = 0; slave < config.slaves; slave++)
            {
                len = recv(connect_fd[slave], buff, BUFFSIZE, 0);
                // lg.recv_message(len, std::string(config.slave_ipadder[slave]), config.slave_port[slave]);
                buff[len] = '\0';
                std::cout << buff << std::endl;
            }
        }
        requests.mtx.unlock(); /** unlock **/
    }
    /*****************************************************/
    
    for (int slave = 0; slave < config.slaves; slave++)
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
    sv_addr.sin_port = htons(config.port);

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

    while (!flag.test())
    {
        len = recv(connect_fd[0], buff, BUFFSIZE, 0);
        // lg.recv_message();
        printf("recv:%d\n", len);

        transaction tx;
        std::stringstream ss;
        ss.write(buff, len);
        cereal::BinaryInputArchive iarchive(ss);
        iarchive(tx);

        // requestsに入れる
        tx.print();
        requests.mtx.lock();
        requests.data.push(tx);
        requests.mtx.unlock();

        // 返答
        sprintf(buff, "ackfrom%d", tx.id);
        ret = send(connect_fd[0], buff, strlen(buff), 0);
        // lg.send_message();
    }
    /******************************************************/

    close(connect_fd[0]);
    close(listen_fd);
}