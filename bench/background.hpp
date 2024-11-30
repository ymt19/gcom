#pragma once

#include "configuration.hpp"
#include "transaction.hpp"
#include <queue>

class background
{
public:
    background(configuration *_config, std::queue<transaction> *_requests);
    void run();
private:
    configuration *config;
    std::queue<transaction> *requests;

    void client(int id);
    void executer(int id);
};
