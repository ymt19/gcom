#pragma once

#include <string>
#include <fstream>

class logger
{
public:
    logger(std::string filename) : stream(filename) {}
    ~logger()
    {
        stream.close();
    }

    void request_transaction();
    void response_transaction();
    void send_message();
    void recv_message();
private:
    std::ofstream stream;
};
