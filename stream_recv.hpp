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
        uint32_t try_pop_packets(unsigned char *data);

    private:
        /**
         * @brief 確認できたidxを登録する
         */
        void register_next_idx();

        uint32_t next_idx; // (next_idx - 1)までのデータは連続してバッファに格納
    };

} // namespace gcom
