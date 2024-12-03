#include "logger.hpp"
#include <syncstream>
#include <iostream>

void logger::request_transaction()
{
    std::cout << "request transaction log" << std::endl;
    std::osyncstream syncout{stream};
    syncout << "request" << std::endl;
}

void logger::response_transaction()
{
    std::osyncstream syncout{stream};
    syncout << "time" << std::endl;
}

void logger::send_message()
{
    std::osyncstream syncout{stream};
    syncout << "time" << std::endl;
}

void logger::recv_message()
{
    std::osyncstream syncout{stream};
    syncout << "time" << std::endl;
}