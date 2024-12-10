#include "timer.hpp"

gcom::timer::timer()
{
    tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
}

gcom::timer::~timer()
{
    close(tfd);
}

int gcom::timer::get_fd()
{
    return tfd;
}