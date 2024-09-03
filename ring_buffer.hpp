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
private:
    static const uint64_t buff_size_ = 4096;
    char body_[buff_size_];
    uint64_t write_idx_;
    uint64_t read_idx_;

    uint64_t get_min_valid_idx()
    {
        return std::max<uint64_t>(0, write_idx_ - buff_size_);
    }

    uint64_t get_max_valid_idx()
    {
        return write_idx_;
    }
public:
    RingBuffer()
    {
        write_idx_ = 0;
        read_idx_ = 0;
    }

    /**
     * Allocates space in the buffer and writes data to it.
     */
    uint64_t push(std::string push_str)
    {
        uint64_t prev_wirte_idx = write_idx_;
        uint64_t remain_size;
        uint64_t offset;
        uint64_t push_size = push_str.size();

        remain_size = buff_size_ - (write_idx_ - read_idx_);
        if (push_size > remain_size)
        {
            return false;
            // エラー処理
        }

        offset = write_idx_ % buff_size_;
        if (offset + push_size > buff_size_)
        {
            std::memcpy((void *)body_ + offset, push_str.c_str(), buff_size_);
            std::memcpy((void *)body_, push_str.c_str() + (buff_size_ - offset), push_size - (buff_size_ - offset));
        }
        else
        {
            std::memcpy((void *)body_ + offset, push_str.c_str(), push_size);
        }

        write_idx_ += push_size;
        return prev_wirte_idx;
    }

    /**
     * Allocates space in the buffer but does not write data to it.
     */
    uint64_t push_empty(uint64_t push_size)
    {
        uint64_t prev_write_idx = write_idx_;
        uint64_t remain_size;

        remain_size = buff_size_ - (write_idx_ - read_idx_);
        if (push_size > remain_size)
        {
            return false;
            // エラー処理
        }

        write_idx_ += push_size;
        return prev_write_idx;
    }

    /**
     * 
     */
    bool pop(uint16_t pop_size)
    {
        if (write_idx_ > read_idx_ + pop_size)
        {
            return false;
            // エラー処理
        }

        read_idx_ += pop_size;
        return true;
    }

    void set(uint64_t idx, std::string set_str)
    {
        uint64_t offset;
        uint64_t set_size = set_str.size();

        if (idx < read_idx_ || idx + set_size > write_idx_)
        {
            return;
            // エラー処理
        }

        offset = idx % buff_size_;
        if (offset + set_size > buff_size_)
        {
            std::memcpy((void *)body_ + offset, set_str.c_str(), buff_size_);
            std::memcpy((void *)body_, set_str.c_str() + (buff_size_ - offset), set_size - (buff_size_ - offset));
        }
        else
        {
            std::memcpy((void *)body_ + offset, set_str.c_str(), set_size);
        }
    }

    std::string get(uint64_t idx, uint16_t get_size)
    {
        std::string get_str(get_size, '\0');
        uint64_t offset;

        if (idx < get_min_valid_idx() || idx + get_size > get_max_valid_idx())
        {
            return nullptr;
            // エラー処理
        }

        offset = idx % buff_size_;
        if (offset + get_size > buff_size_)
        {
            std::memcpy((void *)get_str.c_str(), body_ + offset, buff_size_ - offset);
            std::memcpy((void *)get_str.c_str() + (buff_size_ - offset), body_, get_size - (buff_size_ - offset));
        }
        else
        {
            std::memcpy((void *)get_str.c_str(), body_ + offset, get_size);
        }

        return get_str;
    }
};

}
