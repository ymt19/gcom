#pragma once

#include "ring_buffer.hpp"
#include "endpoint.hpp"
#include <mutex>
#include <map>

namespace gcom
{

    class stream
    {
    public:
        stream(int buffsize) : buff(buffsize) {}

        /**
         * @brief buffからindexのデータを取得
         */
        uint32_t get_packet(uint32_t idx, unsigned char *payload, uint32_t *tail_idx);

        class packet_entry
        {
        public:
            packet_entry(uint32_t payload_size, uint32_t tail_idx)
                : payload_size(payload_size), tail_idx(tail_idx) {}

            uint32_t payload_size;
            uint32_t tail_idx;
        };

        std::mutex mtx;
        ring_buffer buff;
        std::map<uint64_t, packet_entry> info;    // idx : packet_entry
    };

} // namespace gcom
