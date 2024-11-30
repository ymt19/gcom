#include "configuration.hpp"
#include <iostream>

void print_configration(struct configuration *config)
{
    fprintf(stdout, "---------server_config---------\n");
    fprintf(stdout, "server id: %d\n", config->id);
    fprintf(stdout, "ip address: %s\n", config->ipaddr);
    fprintf(stdout, "port: %d\n", config->port);
    fprintf(stdout, "run duration: %d\n", config->duration);

    // only master
    if (config->id == 1)
    {
        fprintf(stdout, "traffic: %d [ops/sec]\n", config->iops);
        fprintf(stdout, "number of client threads: %d\n", config->threads);
        fprintf(stdout, "send log size: %d [bytes]\n", config->log_size);
        fprintf(stdout, "number of slaves: %d\n", config->slaves);
        for (int slave = 0; slave < config->slaves; slave++) {
            fprintf(stdout, "slave: ip address: %s, port: %d\n", config->slave_ipadder[slave], config->slave_port[slave]);
        }
    }
    fprintf(stdout, "-------------------------------\n");
}