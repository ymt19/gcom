#pragma once

#include <sys/timerfd.h>
#include <unistd.h>

namespace gcom
{
    class timeout_manager
    {
    public:
        timeout_manager();
        ~timeout_manager();

        int get_fd();
    private:
        int tfd;
    };
    
} // namespace gcom
