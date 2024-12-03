#pragma once

#include "configuration.hpp"
#include "transaction.hpp"
#include <queue>
#include <thread>

class tcp_connection_manager
{
public:
    tcp_connection_manager(configuration& _config, txqueue& _requests);
    ~tcp_connection_manager();
private:
    configuration& config;
    txqueue& requests;
    std::thread worker;
    int connect_fd[10];
    int listen_fd;

    void sender();
    void receiver();
};