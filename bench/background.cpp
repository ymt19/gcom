#include "background.hpp"
#include <iostream>
#include <vector>
#include <thread>

background::background(configuration* _config, std::queue<transaction> *_requests)
{
    config = _config;
    requests = _requests;
}

void background::run()
{
    // start

    if (config->id == 1) // master
    {
        std::vector<std::thread> threads;
        for (int id = 1; id <= config->slaves; id++)
        {
            threads.emplace_back(std::thread(client, id));
        }

        // wait duration
    
        for (auto& thread : threads)
        {
            thread.join();
        }
    }
    else // slaves
    {
        // queueから取り出す
        // logに書き込む
    }
}

void background::client(int id)
{
    // tx生成
    // queueに入れる
    // logに書き込む
}