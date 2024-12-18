#pragma once

#include "stream.hpp"

namespace gcom
{

    class stream_send : public stream
    {
    public:
        /**
         * @brief データをパケットに分割してbuffに挿入
         */
        void push_packets(unsigned char *data, uint32_t len, uint32_t max_payload_size);

        /**
         * @brief tail_idxまでのデータをbuffから削除
         */
        void pop_packets(uint32_t tail_idx);
    };

} // namespace gcom
