#include "ring_buffer.hpp"

ring_buffer::ring_buffer()
{
    body = new unsigned char[default_size];
    size = default_size;
    write_idx = 0;
    read_idx = 0;
}

uint64_t ring_buffer::push(unsigned char *data, uint64_t len)
{
    uint64_t offset;
    uint64_t prev_write_idx;

    if (len > this->size - (this->write_idx - this->read_idx))
    {
        throw std::exception();
    }

    offset = this->write_idx % this->size;
    if (offset + len > this->size)
    {
        std::memcpy((void *)(this->body + offset), data, this->size);
        std::memcpy((void *)this->body, data + (this->size - offset), len - (this->size - offset));
    }
    else
    {
        std::memcpy((void *)(this->body + offset), data, len);
    }

    prev_write_idx = this->write_idx;
    this->write_idx += len;
    return prev_write_idx;
}

uint64_t ring_buffer::pop(unsigned char *data, uint64_t len)
{
    std::string pop_str;
    try
    {
        get(this->read_idx, data, len);
    }
    catch(const std::exception& e)
    {
        throw std::exception();
    }
    

    this->read_idx += len;
    return this->read_idx;
}

void ring_buffer::set(uint64_t idx, unsigned char *data, uint64_t len)
{
    uint64_t offset;

    if (idx < this->read_idx || idx + len > this->write_idx)
    {
        throw std::exception();
    }

    offset = idx % this->size;
    if (offset + len > this->size)
    {
        std::memcpy((void *)(this->body + offset), data, this->size);
        std::memcpy((void *)this->body, data + (this->size - offset), len - (this->size - offset));
    }
    else
    {
        std::memcpy((void *)(this->body + offset), data, len);
    }
}

void ring_buffer::get(uint64_t idx, unsigned char *data, uint64_t len)
{
    uint64_t offset;

    if (idx < get_min_valid_idx() || idx + len > get_max_valid_idx())
    {
        throw std::exception();
    }

    offset = idx % this->size;
    if (offset + len > this->size)
    {
        std::memcpy((void *)data, this->body + offset, this->size - offset);
        std::memcpy((void *)(data + (this->size - offset)), this->body, len - (this->size - offset));
    }
    else
    {
        std::memcpy((void *)data, this->body + offset, len);
    }
}

uint64_t ring_buffer::get_min_valid_idx()
{
    if (this->write_idx < this->size)
    {
        return 0;
    }
    else
    {
        return this->write_idx - this->size;
    }
}

uint64_t ring_buffer::get_max_valid_idx()
{
    return this->write_idx;
}