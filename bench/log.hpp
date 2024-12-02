#pragma once

#include <string>
#include <fstream>

class log
{
public:
    log(std::string _filename);
    ~log();
private:
    std::fstream stream;
};
