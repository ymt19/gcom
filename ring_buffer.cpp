#include "ring_buffer.hpp"

uint64_t gcom::ring_buffer::push(unsigned char *data, uint64_t len)
{
    uint64_t offset;
    uint64_t prev_write_idx;

    if (len > size - (write_idx - read_idx))
    {
        throw std::exception();
    }

    offset = write_idx % size;
    if (offset + len > size)
    {
        std::memcpy((void *)(body + offset), data, size);
        std::memcpy((void *)body, data + (size - offset), len - (size - offset));
    }
    else
    {
        std::memcpy((void *)(body + offset), data, len);
    }


    prev_write_idx = write_idx;
    write_idx += len;

    if (write_idx - zombie_idx > size)
    {
        // calc zombie index
        zombie_idx = write_idx - size;
    }

    return prev_write_idx;
}

uint64_t gcom::ring_buffer::push_empty(uint64_t len)
{
    uint64_t prev_write_idx;

    if (len > size - (write_idx - read_idx))
    {
        throw std::exception();
    }
    
    prev_write_idx = write_idx;
    write_idx += len;

    if (write_idx - zombie_idx > size)
    {
        // calc zombie index
        zombie_idx = write_idx - size;
    }

    return prev_write_idx;
}

uint64_t gcom::ring_buffer::pop(uint64_t len)
{
    if (len > write_idx - read_idx)
    {
        throw std::exception();
    }

    read_idx += len;
    return read_idx;
}

void gcom::ring_buffer::set(uint64_t idx, unsigned char *data, uint64_t len)
{
    uint64_t offset;

    if (idx < read_idx || idx + len > write_idx)
    {
        throw std::exception();
    }

    offset = idx % size;
    if (offset + len > size)
    {
        std::memcpy((void *)(body + offset), data, size);
        std::memcpy((void *)body, data + (size - offset), len - (size - offset));
    }
    else
    {
        std::memcpy((void *)(body + offset), data, len);
    }
}

void gcom::ring_buffer::get(uint64_t idx, unsigned char *data, uint64_t len)
{
    uint64_t offset;

    if (idx < zombie_idx || idx > write_idx - len)
    {
        throw std::exception();
    }

    offset = idx % size;
    if (offset + len > size)
    {
        std::memcpy((void *)data, body + offset, size - offset);
        std::memcpy((void *)(data + (size - offset)), body, len - (size - offset));
    }
    else
    {
        std::memcpy((void *)data, body + offset, len);
    }
}
