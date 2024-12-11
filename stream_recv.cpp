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

uint32_t gcom::stream_recv::pop_packets(unsigned char *data)
{
    uint64_t read_idx, top_tail_idx;

    std::lock_guard<std::mutex> lock(mtx);

    read_idx = buff.get_read_idx();
    top_tail_idx = info.at(read_idx).tail_idx;

    if (confirmed_idx >= top_tail_idx)
    {
        uint32_t len = top_tail_idx - read_idx;
        buff.get(read_idx, data, len);
        buff.pop(len);
        return len;
    }
    else
    {
        return 0;
    }
}

void gcom::stream_recv::register_confirmed_idx(uint32_t idx, )
{
    std::lock_guard<std::mutex> lock(mtx);

    // calculate new confirmed_idx
    auto itr = info.find(idx);
    while (itr != info.end())
    {
        if (itr->second.is_confirmed)
        {

        }
        else
        {

        }
    }
}