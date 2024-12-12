#include "stream_send.hpp"

void gcom::stream_send::push_packets(unsigned char *data, uint32_t len, uint32_t max_payload_size)
{
    uint32_t idx, head_idx, tail_idx;

    std::lock_guard<std::mutex> lock(mtx);

    idx = buff.push_empty(len);
    head_idx = idx;
    tail_idx = head_idx + len;

    // 分割して挿入
    while(idx != tail_idx)
    {
        uint32_t payload_size = std::min(max_payload_size, tail_idx - idx);
        buff.set(idx, data + (idx - head_idx), payload_size);
        info.emplace(idx, packet_entry(payload_size, tail_idx));
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

void gcom::stream_send::pop_packets(uint32_t tail_idx)
{
    uint64_t read_idx;

    std::lock_guard<std::mutex> lock(mtx);

    read_idx = buff.get_read_idx();

    if (read_idx < tail_idx)
    {
        uint32_t len = tail_idx - read_idx;
        buff.pop(len);
    }
}