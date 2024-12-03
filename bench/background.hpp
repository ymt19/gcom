#pragma once

#include "configuration.hpp"
#include "transaction.hpp"
#include "logger.hpp"
#include <queue>

class background
{
public:
    background(configuration& _config, txqueue& _requests) : config(_config), requests(_requests) {}
    void run(logger& lg);
private:
    configuration& config;
    txqueue& requests;

    void client(logger& lg, int id);
    void executer(logger& lg, int id);
};
