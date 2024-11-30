#include "background.hpp"
#include <iostream>

background::background(std::queue<transaction> *_requests)
{
    requests = _requests;
    std::cout << "run" << std::endl;
}

background::~background()
{

}

void background::run(configuration& config)
{
    
}

void background::client()
{

}