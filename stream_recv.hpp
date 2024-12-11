#pragma once

#include "stream.hpp"

namespace gcom
{

    class stream_recv : public stream
    {
    public:
        /**
         * @brief buffにデータを挿入（receiver）
         */
        void insert_packet(unsigned char *payload, uint32_t len, uint32_t idx, uint32_t tail_idx);

        /**
         * @brief buffからconfirmed packetsを削除（sender/receiver）
         */
        uint32_t pop_packets(unsigned char *data);

    private:
        /**
         * @brief 確認できたidxを登録する（sender/receiver）
         * insert_packet()で呼出
         */
        void register_confirmed_idx(uint32_t idx);

        ssize_t confirmed_idx; // このindexまでのデータは連続して格納されている
    };

} // namespace gcom
