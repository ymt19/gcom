#include "ring_buffer.hpp"

uint64_t multicast::RingBuffer::get_min_valid_idx()
{
    if (write_idx_ < BUFFER_SIZE)
    {
        return 0;
    }
    else
    {
        return write_idx_ - BUFFER_SIZE;
    }
}

uint64_t multicast::RingBuffer::get_max_valid_idx()
{
    return write_idx_;
}

multicast::RingBuffer::RingBuffer()
{
    write_idx_ = 0;
    read_idx_ = 0;
}

uint64_t multicast::RingBuffer::push(std::string push_str)
{
    uint64_t offset;
    uint64_t push_size = push_str.size();

    if (push_size > BUFFER_SIZE - (write_idx_ - read_idx_))
    {
        throw std::exception();
    }

    offset = write_idx_ % BUFFER_SIZE;
    if (offset + push_size > BUFFER_SIZE)
    {
        std::memcpy((void *)(body_ + offset), push_str.c_str(), BUFFER_SIZE);
        std::memcpy((void *)body_, push_str.c_str() + (BUFFER_SIZE - offset), push_size - (BUFFER_SIZE - offset));
    }
    else
    {
        std::memcpy((void *)(body_ + offset), push_str.c_str(), push_size);
    }

    write_idx_ += push_size;
    return write_idx_;
}

uint64_t multicast::RingBuffer::push_empty(uint64_t push_size)
{
    if (push_size > BUFFER_SIZE - (write_idx_ - read_idx_))
    {
        throw std::exception();
    }

    write_idx_ += push_size;
    return write_idx_;
}

std::string multicast::RingBuffer::pop(uint64_t pop_size)
{
    std::string pop_str;
    try
    {
        pop_str = get(read_idx_, pop_size);
    }
    catch(const std::exception& e)
    {
        throw std::exception();
    }
    

    read_idx_ += pop_size;
    return pop_str;
}

void multicast::RingBuffer::set(uint64_t idx, std::string set_str)
{
    uint64_t offset;
    uint64_t set_size = set_str.size();

    if (idx < read_idx_ || idx + set_size > write_idx_)
    {
        throw std::exception();
    }

    offset = idx % BUFFER_SIZE;
    if (offset + set_size > BUFFER_SIZE)
    {
        std::memcpy((void *)(body_ + offset), set_str.c_str(), BUFFER_SIZE);
        std::memcpy((void *)body_, set_str.c_str() + (BUFFER_SIZE - offset), set_size - (BUFFER_SIZE - offset));
    }
    else
    {
        std::memcpy((void *)(body_ + offset), set_str.c_str(), set_size);
    }
}

std::string multicast::RingBuffer::get(uint64_t idx, uint64_t get_size)
{
    std::string get_str(get_size, '\0');
    uint64_t offset;

    if (idx < get_min_valid_idx() || idx + get_size > get_max_valid_idx())
    {
        throw std::exception();
    }

    offset = idx % BUFFER_SIZE;
    if (offset + get_size > BUFFER_SIZE)
    {
        std::memcpy((void *)get_str.c_str(), body_ + offset, BUFFER_SIZE - offset);
        std::memcpy((void *)(get_str.c_str() + (BUFFER_SIZE - offset)), body_, get_size - (BUFFER_SIZE - offset));
    }
    else
    {
        std::memcpy((void *)get_str.c_str(), body_ + offset, get_size);
    }

    return get_str;
}