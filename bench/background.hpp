#pragma once

#include "configuration.hpp"
#include "transaction.hpp"
#include <queue>

class background
{
public:
    background(std::queue<transaction> *_requests);
    ~background();
    void run(configuration& config);
private:
    std::queue<transaction> *requests;

    void client();
};
