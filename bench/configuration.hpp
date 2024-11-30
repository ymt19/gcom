#pragma once

struct configuration
{
    int id;
    char ipaddr[32];
    int port;
    int duration; // sec

    // master
    int iops; // ops/sec
    int threads;
    int log_size; // bytes
    int slaves;
    char slave_ipadder[10][32];
    int slave_port[10];
};

void print_configration(struct configuration *config);