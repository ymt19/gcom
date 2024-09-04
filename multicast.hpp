#pragma once

#include "socket.hpp"
#include "ring_buffer.hpp"

namespace multicast
{

// For Sender.
class SenderSocket : public Socket
{
private:
    int genseq_;
    void bg();
public:
    SenderSocket();
    void send();
};

// For Receiver.
class ReceiverSocket : public Socket
{
private:
public:
    ReceiverSocket();
    void recv();
};

}
