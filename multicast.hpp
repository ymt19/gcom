#pragma once

#include "socket.hpp"
#include "ring_buffer.hpp"

namespace multicast
{

// For Sender.
class SenderSocket : public Socket
{
public:
    SenderSocket();
    void send();
private:
    void background();

    int _generated_seq;
};

// For Receiver.
class ReceiverSocket : public Socket
{
public:
    ReceiverSocket();
    void recv();
private:
    void background();
};

class DistributionTree
{
public:
private:
    int _id;
}

}
