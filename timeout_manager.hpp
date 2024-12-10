#pragma once

#include <sys/timerfd.h>
#include <unistd.h>

namespace gcom
{
    class timer
    {
    public:
        timer();
        ~timer();

        int get_fd();
    private:
        int tfd;
    };
    
} // namespace gcom
