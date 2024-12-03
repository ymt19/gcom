#pragma once

#include "configuration.hpp"
#include "transaction.hpp"
#include "logger.hpp"
#include <queue>
#include <thread>

class tcp_connection_manager
{
public:
    tcp_connection_manager(configuration& config, txqueue& requests, logger& lg);
    ~tcp_connection_manager();
private:
    configuration& config;
    txqueue& requests;
    logger& lg;
    std::thread worker;
    int connect_fd[10];
    int listen_fd;

    void sender();
    void receiver();
};