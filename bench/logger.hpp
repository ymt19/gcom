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

    void request_transaction(int txid, int clientid);
    void response_transaction(int txid, int clientid);
    void send_message(int size, std::string fromipaddr, int fromport);
    void recv_message(int size, std::string toipaddr, int toport);
    inline double get_time();
private:
    enum type
    {
        type_request_transaction = 1,
        type_response_transaction = 2,
        type_send_message = 3,
        type_recv_message = 4,
    };

    std::ofstream stream;
};
