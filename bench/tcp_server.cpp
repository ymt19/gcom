#include "configuration.hpp"
#include "tcp_connection_manager.hpp"
#include "background.hpp"
#include <iostream>
#include <string.h>

// ./tcp_server <id> <ipaddr:port> <duration[sec]> <input traffic[ops/sec]> <number of threads> <log size[bytes]> <slaves> <server2 ipaddr:port> <server3 ipaddr:port>...

int main(int argc, char *argv[]) {
    std::queue<transaction> requests;

    // config生成
    configuration config(argv);
    config.print();

    // run connection manager
    tcp_connection_manager tcm(&config, &requests);
    tcm.run();

    // run client thread
    background bg(&config, &requests);
    bg.run();
}