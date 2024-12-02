#pragma once

#include "configuration.hpp"
#include "transaction.hpp"
#include <queue>

class background
{
public:
    background(configuration *_config, struct requests *_reqs);
    void run();
private:
    configuration *config;
    struct requests *reqs;

    void client(int id);
    void executer(int id);
};
