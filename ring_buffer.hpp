#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <cstring>
#include <algorithm>

namespace multicast
{

class RingBuffer
{
public:
    RingBuffer();

    // Allocates space in the buffer and writes data to it.
    // Returns new write index. 
    uint64_t push(unsigned char *data, uint64_t size);

    // Return new read index.
    uint64_t pop(unsigned char *data, uint64_t size);

    //
    void set(uint64_t idx, unsigned char *data, uint64_t size);

    //
    void get(uint64_t idx, unsigned char *data, uint64_t size);
private:
    //
    uint64_t get_min_valid_idx();

    //
    uint64_t get_max_valid_idx();

    const uint64_t default_buffer_size = 4096;

    unsigned char *body_;
    uint64_t buffer_size_;
    uint64_t write_idx_;
    uint64_t read_idx_;
};

}
