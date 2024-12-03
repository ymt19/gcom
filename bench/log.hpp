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
private:
    std::ofstream stream;
};
