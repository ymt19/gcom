#include "multicast.hpp"

namespace multicast
{

Socket::Socket(uint16_t port)
    : udp_sock_(io_context_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
    generated_seq_(0) {}

Socket::~Socket()
{

}

void Socket::send(std::string msg, boost::asio::ip::udp::endpoint dest)
{
    // udp_sock_.send_to(asio::buffer(&msg, size, dest));
}

void Socket::recv()
{

}

void Socket::output_packet()
{

}

void Socket::input_packet()
{

}

void Socket::background()
{

}

} // namespace multicast