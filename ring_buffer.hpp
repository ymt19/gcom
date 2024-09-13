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
    uint64_t push(std::string push_str);

    // Allocates space in the buffer but does not write data to it.
    uint64_t push_empty(uint64_t push_size);

    // 
    std::string pop(uint64_t pop_size);

    //
    void set(uint64_t idx, std::string set_str);

    //
    std::string get(uint64_t idx, uint64_t get_size);
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
