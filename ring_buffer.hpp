#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <cstring>
#include <algorithm>

namespace gcom
{

class ring_buffer
{
public:
    ring_buffer(uint64_t size) : size(size), write_idx(0), read_idx(0), zombie_idx(0)
    {
        body = new unsigned char[size];
    }

    // Allocates space in the buffer and writes data to it.
    // Returns prev write index. 
    uint64_t push(unsigned char *data, uint64_t len);

    // Return new read index.
    uint64_t pop(unsigned char *data, uint64_t len);

    // don't update write index.
    void set(uint64_t idx, unsigned char *data, uint64_t len);

    //
    void get(uint64_t idx, unsigned char *data, uint64_t len);
private:
    unsigned char *body;
    uint64_t size;
    uint64_t write_idx;
    uint64_t read_idx;
    uint64_t zombie_idx;
};

} // namespace gcom
