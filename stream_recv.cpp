#include "stream_recv.hpp"

void gcom::stream_recv::insert_packet(unsigned char *payload, uint32_t len, uint32_t idx, uint32_t tail_idx)
{
    std::lock_guard<std::mutex> lock(mtx);

    if (tail_idx > buff.get_write_idx())
    {
        buff.push_empty(tail_idx - buff.get_write_idx());
    }

    buff.set(idx, payload, len);
    info.emplace(idx, packet_entry(len, tail_idx));

    register_next_idx();

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

uint32_t gcom::stream_recv::try_pop_packets(unsigned char *data)
{
    uint64_t read_idx, tail_idx;

    std::lock_guard<std::mutex> lock(mtx);

    read_idx = buff.get_read_idx();
    tail_idx = info.at(read_idx).tail_idx;

    if (next_idx  > tail_idx)
    {
        uint32_t len = tail_idx - read_idx;
        buff.get(read_idx, data, len);
        buff.pop(len);
        return len;
    }
    else
    {
        return 0;
    }
}

void gcom::stream_recv::register_next_idx()
{
    auto itr = info.find(next_idx);
    while (itr != info.end())
    {
        if (next_idx == itr->first)
        {
            next_idx += itr->second.payload_size;
        }
        else
        {
            break;
        }
    }
}