#include "background.hpp"
#include <iostream>
#include <vector>
#include <thread>

background::background(configuration *_config, std::queue<transaction> *_requests)
{
    config = _config;
    requests = _requests;
}

void background::run()
{
    std::vector<std::thread> threads;
    if (config->id == 1) // master
    {
        // start

        for (int id = 1; id <= config->threads; id++)
        {
            threads.emplace_back(std::thread(&background::client, this, id));
        }

        // wait duration
        // throw;
    
        for (auto& thread : threads)
        {
            thread.join();
        }
    }
    else // slaves
    {
        // start

        threads.emplace_back(std::thread(&background::executer, this, 1));

        // wait duration
        // throw;

        threads.back().join();
    }
}

void background::client(int id)
{
    try
    {
        // 1. tx生成
        // 2. queueから取り出す
        // 3. logに書き込む
        for (int i = 0; i < 10; i++)
        {
            printf("duration:%d id:%d cnt:%d\n", config->duration, id, i);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void background::executer(int id)
{
    // queueから取り出す
    // logに書き込む
}