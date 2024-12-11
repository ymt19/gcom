#include "stream.hpp"

uint32_t gcom::stream::get_packet(uint32_t idx, unsigned char *payload, uint32_t *tail_idx)
{
    std::lock_guard<std::mutex> lock(mtx);
    
    auto itr = info.find(idx);
    if (itr == info.end())
    {
        throw std::exception();
    }

    *tail_idx = itr->second.tail_idx;
    buff.get(idx, payload, itr->second.payload_size);
    return itr->second.payload_size;
}
