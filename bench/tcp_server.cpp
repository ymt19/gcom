#include "configuration.hpp"
#include <iostream>
#include <string.h>

// ./tcp_server <id> <ipaddr:port> <duration[sec]> <input traffic[ops/sec]> <number of threads> <log size[bytes]> <slaves> <server2 ipaddr:port> <server3 ipaddr:port>...

void parse_arguments(struct configuration *config, char *argv[])
{
    config->id = atoi(argv[1]);
    strncpy(config->ipaddr, argv[2], strlen(argv[2]));
    config->ipaddr[strlen(argv[2])] = '\0';
    config->port = atoi(argv[3]);
    config->duration = atoi(argv[4]);

    // only master
    if (config->id == 1)
    {
        config->iops = atoi(argv[5]);
        config->threads = atoi(argv[6]);
        config->log_size = atoi(argv[7]);
        config->slaves = atoi(argv[8]);

        for (int slave = 0; slave < config->slaves; slave++)
        {
            int idx = 9 + slave*2;
            strncpy(config->slave_ipadder[slave], argv[idx], strlen(argv[idx]));
            config->slave_ipadder[slave][strlen(argv[idx])] = '\0';
            config->slave_port[slave] = atoi(argv[idx+1]);
        }
    }
}

int main(int argc, char *argv[]) {
    struct configuration config;
    // config生成
    parse_arguments(&config, argv);
    print_configration(&config);

    // run connection manager


    // run client thread

}