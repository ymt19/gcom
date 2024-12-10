#include "stream.hpp"

void gcom::stream::push_packets(unsigned char *data, uint32_t len, uint32_t max_payload_size)
{
    uint32_t idx, head_idx, tail_idx;

    std::lock_guard<std::mutex> lock(mtx);

    head_idx = buff.get_write_idx();
    tail_idx = head_idx + len;
    idx = head_idx;

    // 分割して挿入
    while(idx != tail_idx)
    {
        uint32_t payload_size = std::min(max_payload_size, tail_idx - head_idx);
        uint64_t idx = buff.push(data + (idx - head_idx), payload_size);
        info.emplace(idx, packet(payload_size, head_idx, tail_idx));

        idx += payload_size;
    }

    // zombie index未満のidxに格納されるパケット情報をinfoから削除
    auto itr = info.begin();
    while (itr != info.end())
    {
        if (buff.get_zombie_idx() <= itr->first)
        {
            break;
        }
        itr = info.erase(itr);
    }
}

void gcom::stream::insert_packet(unsigned char *data, uint32_t len, uint32_t idx, uint32_t head_idx, uint32_t tail_idx)
{
    std::lock_guard<std::mutex> lock(mtx);

    if (tail_idx > buff.get_write_idx())
    {
        buff.push_empty(tail_idx - buff.get_write_idx());
    }

    buff.set(idx, data, len);
    info.emplace(idx, packet(len, head_idx, tail_idx));

    if (next_idx == idx)
    {
        next_idx += len;
        auto itr = info.find(next_idx);
        while (itr != info.end())
        {
            if (next_idx == itr->first)
            {
                next_idx += itr->second.payload_size;
                itr++;
            } else
            {
                break;
            }
        }
    }
}

uint32_t gcom::stream::pop_packets(unsigned char *data)
{
    uint64_t read_idx, len;

    std::lock_guard<std::mutex> lock(mtx);

    read_idx = buff.get_read_idx();
    len = info.at(read_idx).tail_idx - info.at(read_idx).head_idx;
    if (next_idx > read_idx + len)
    {
        buff.get(read_idx, data, len);
        return len;
    }
    else
    {
        return 0;
    }
}

uint32_t gcom::stream::get_packet(uint32_t idx, unsigned char *data, uint32_t *head_idx, uint32_t *tail_idx)
{
    std::lock_guard<std::mutex> lock(mtx);

    if (info.empty())
    {
        throw std::exception();
    }
    
    auto itr = info.find(idx);
    if (itr == info.end())
    {
        throw std::exception();
    }
    
    *head_idx = itr->second.head_idx;
    *tail_idx = itr->second.tail_idx;
    buff.get(idx, data, itr->second.payload_size);
    return itr->second.payload_size;
}