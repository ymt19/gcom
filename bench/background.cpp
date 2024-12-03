#include "background.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

background::background(configuration *_config, struct requests *_reqs)
{
    config = _config;
    reqs = _reqs;
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
        // atomic variable flag set
    
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
        // atomic variable flag set

        threads.back().join();
    }
}

void background::client(int id)
{
    int txid = 0;
    int client = id;
    int size = 0;
    for (int i = 0; i < 10; i++)
    {
        // op/sec
        std::this_thread::sleep_for(std::chrono::seconds(1));
        txid++;

        reqs->mtx.lock();
        reqs->data.push(transaction(txid, client, size));
        reqs->mtx.unlock();
    }
}

void background::executer(int id)
{
    // queueから取り出す
    // logに書き込む
}