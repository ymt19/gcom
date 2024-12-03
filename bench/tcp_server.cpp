#include "configuration.hpp"
#include "tcp_connection_manager.hpp"
#include "background.hpp"
#include "transaction.hpp"
#include "logger.hpp"
#include <iostream>
#include <string>

// ./tcp_server <id> <ipaddr:port> <duration[sec]> <input traffic[ops/sec]> <number of threads> <log size[bytes]> <slaves> <server2 ipaddr:port> <server3 ipaddr:port>...

int main(int argc, char *argv[]) {
    txqueue requests;

    // config生成
    configuration config(argv);
    config.print();

    // logger生成
    std::string logfilename = "log" + config.id;
    logger lg(logfilename);

    // run connection manager
    tcp_connection_manager tcm(config, requests);

    // run client thread
    background bg(config, requests);
    bg.run(lg);
}