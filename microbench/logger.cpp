#include "logger.hpp"
#include <syncstream>
#include <iostream>
#include <chrono>

void logger::request_transaction(int txid, int clientid)
{
    std::osyncstream syncout{stream};
    syncout << get_time() << ",";
    syncout << type_request_transaction << ",";
    syncout << txid << ",";
    syncout << clientid << ",";
    syncout << std::endl;
}

void logger::response_transaction(int txid, int clientid)
{
    std::osyncstream syncout{stream};
    syncout << get_time() << ",";
    syncout << type_response_transaction << ",";
    syncout << txid << ",";
    syncout << clientid << ",";
    syncout << std::endl;
}

void logger::send_message(int size, std::string fromipaddr, int fromport)
{
    std::osyncstream syncout{stream};
    syncout << get_time() << ",";
    syncout << type_send_message << ",";
    syncout << size << ",";
    syncout << fromipaddr << ",";
    syncout << fromport << ",";
    syncout << std::endl;
}

void logger::recv_message(int size, std::string toipaddr, int toport)
{
    std::osyncstream syncout{stream};
    syncout << get_time() << ",";
    syncout << type_recv_message << ",";
    syncout << size << ",";
    syncout << toipaddr << ",";
    syncout << toport << ",";
    syncout << std::endl;
}

inline double logger::get_time()
{
    using namespace std::chrono;
    const system_clock::time_point tp = system_clock::now();
    return static_cast<double>(duration_cast<milliseconds>(tp.time_since_epoch().count())) / 1000000;
}