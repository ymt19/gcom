#pragma once

#include <pthread.h>
#include <buffer.hpp>

class sender_socket
{
private:
    int sockfd;
    int sigfd;
    pthread_t bg_threadid;
    int genseq;
    buffer buff;
public:
    sender_socket(/* args */);
    ~sender_socket();
    
};

class receiver_socket
{
private:

public:

};