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

uint64_t RingBuffer::push(unsigned char *data, uint64_t size)
{
    uint64_t offset;

    if (size > buffer_size_ - (write_idx_ - read_idx_))
    {
        throw std::exception();
    }

    offset = write_idx_ % buffer_size_;
    if (offset + size > buffer_size_)
    {
        std::memcpy((void *)(body_ + offset), data, buffer_size_);
        std::memcpy((void *)body_, data + (buffer_size_ - offset), size - (buffer_size_ - offset));
    }
    else
    {
        std::memcpy((void *)(body_ + offset), data, size);
    }

    write_idx_ += size;
    return write_idx_;
}

uint64_t RingBuffer::pop(unsigned char *data, uint64_t size)
{
    std::string pop_str;
    try
    {
        get(read_idx_, data, size);
    }
    catch(const std::exception& e)
    {
        throw std::exception();
    }
    

    read_idx_ += size;
    return read_idx_;
}

void RingBuffer::set(uint64_t idx, unsigned char *data, uint64_t size)
{
    uint64_t offset;

    if (idx < read_idx_ || idx + size > write_idx_)
    {
        throw std::exception();
    }

    offset = idx % buffer_size_;
    if (offset + size > buffer_size_)
    {
        std::memcpy((void *)(body_ + offset), data, buffer_size_);
        std::memcpy((void *)body_, data + (buffer_size_ - offset), size - (buffer_size_ - offset));
    }
    else
    {
        std::memcpy((void *)(body_ + offset), data, size);
    }
}

void RingBuffer::get(uint64_t idx, unsigned char *data, uint64_t size)
{
    uint64_t offset;

    if (idx < get_min_valid_idx() || idx + size > get_max_valid_idx())
    {
        throw std::exception();
    }

    offset = idx % buffer_size_;
    if (offset + size > buffer_size_)
    {
        std::memcpy((void *)data, body_ + offset, buffer_size_ - offset);
        std::memcpy((void *)(data + (buffer_size_ - offset)), body_, size - (buffer_size_ - offset));
    }
    else
    {
        std::memcpy((void *)data, body_ + offset, size);
    }
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