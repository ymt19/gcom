#include "background.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

void background::run(logger& lg)
{
    std::vector<std::thread> threads;
    if (config.id == 1) // master
    {
        // start

        for (int id = 1; id <= config.threads; id++)
        {
            threads.emplace_back(std::thread(&background::client, this, std::ref(lg), id));
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

        threads.emplace_back(std::thread(&background::executer, this, std::ref(lg), 1));

        // wait duration
        // atomic variable flag set

        threads.back().join();
    }
}

void background::client(logger& lg, int id)
{
    int txid = 0;
    int client = id;
    int size = 0;
    for (int i = 0; i < 10; i++)
    {
        // op/sec
        std::this_thread::sleep_for(std::chrono::seconds(1));
        txid++;

        requests.mtx.lock();
        requests.data.push(transaction(txid, client, size));
        lg.request_transaction(txid, client);
        requests.mtx.unlock();
    }
}

void background::executer(logger& lg, int id)
{
    while (1)
    {
        requests.mtx.lock();
        if (!requests.data.empty())
        {
            transaction tx = requests.data.front();
            lg.request_transaction(tx.id, tx.client);
            requests.data.pop();
        }
        requests.mtx.unlock();
    }
}