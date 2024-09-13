#include "ring_buffer.hpp"

namespace multicast
{

RingBuffer::RingBuffer()
{
    body_ = new unsigned char[default_buffer_size];
    buffer_size_ = default_buffer_size;
    write_idx_ = 0;
    read_idx_ = 0;
}

uint64_t RingBuffer::push(std::string push_str)
{
    uint64_t offset;
    uint64_t push_size = push_str.size();

    if (push_size > buffer_size_ - (write_idx_ - read_idx_))
    {
        throw std::exception();
    }

    offset = write_idx_ % buffer_size_;
    if (offset + push_size > buffer_size_)
    {
        std::memcpy((void *)(body_ + offset), push_str.c_str(), buffer_size_);
        std::memcpy((void *)body_, push_str.c_str() + (buffer_size_ - offset), push_size - (buffer_size_ - offset));
    }
    else
    {
        std::memcpy((void *)(body_ + offset), push_str.c_str(), push_size);
    }

    write_idx_ += push_size;
    return write_idx_;
}

uint64_t RingBuffer::push_empty(uint64_t push_size)
{
    if (push_size > buffer_size_ - (write_idx_ - read_idx_))
    {
        throw std::exception();
    }

    write_idx_ += push_size;
    return write_idx_;
}

std::string RingBuffer::pop(uint64_t pop_size)
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

void RingBuffer::set(uint64_t idx, std::string set_str)
{
    uint64_t offset;
    uint64_t set_size = set_str.size();

    if (idx < read_idx_ || idx + set_size > write_idx_)
    {
        throw std::exception();
    }

    offset = idx % buffer_size_;
    if (offset + set_size > buffer_size_)
    {
        std::memcpy((void *)(body_ + offset), set_str.c_str(), buffer_size_);
        std::memcpy((void *)body_, set_str.c_str() + (buffer_size_ - offset), set_size - (buffer_size_ - offset));
    }
    else
    {
        std::memcpy((void *)(body_ + offset), set_str.c_str(), set_size);
    }
}

std::string RingBuffer::get(uint64_t idx, uint64_t get_size)
{
    std::string get_str(get_size, '\0');
    uint64_t offset;

    if (idx < get_min_valid_idx() || idx + get_size > get_max_valid_idx())
    {
        throw std::exception();
    }

    offset = idx % buffer_size_;
    if (offset + get_size > buffer_size_)
    {
        std::memcpy((void *)get_str.c_str(), body_ + offset, buffer_size_ - offset);
        std::memcpy((void *)(get_str.c_str() + (buffer_size_ - offset)), body_, get_size - (buffer_size_ - offset));
    }
    else
    {
        std::memcpy((void *)get_str.c_str(), body_ + offset, get_size);
    }

    return get_str;
}

uint64_t RingBuffer::get_min_valid_idx()
{
    if (write_idx_ < buffer_size_)
    {
        return 0;
    }
    else
    {
        return write_idx_ - buffer_size_;
    }
}

uint64_t RingBuffer::get_max_valid_idx()
{
    return write_idx_;
}

} // namespace multicast