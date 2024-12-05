#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <cstring>
#include <algorithm>

class ring_buffer
{
public:
    ring_buffer(uint64_t size) : size(size), write_idx(0), read_idx(0)
    {
        body = new unsigned char[size];
    }

    // Allocates space in the buffer and writes data to it.
    // Returns prev write index. 
    uint64_t push(unsigned char *data, uint64_t len);

    // Return new read index.
    uint64_t pop(unsigned char *data, uint64_t len);

    //
    void set(uint64_t idx, unsigned char *data, uint64_t len);

    //
    void get(uint64_t idx, unsigned char *data, uint64_t len);
private:
    //
    uint64_t get_min_valid_idx();

    //
    uint64_t get_max_valid_idx();

    unsigned char *body;
    uint64_t size;
    uint64_t write_idx;
    uint64_t read_idx;
};
