#include "socket.hpp"

void multicast::Socket::output_packet()
{

}

void multicast::Socket::input_packet()
{

}

void multicast::Socket::background()
{

}

multicast::Socket::Socket(int port)
{
    _io_service = boost::asio::io_service();
    _udp_sock = ip::udp::socket(_io_service, ip::udp::endpoint(ip::udp::v4(), port));

}