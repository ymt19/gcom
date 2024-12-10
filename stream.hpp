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
    stream(int buffsize)
        : buff(buffsize), next_idx(0) {}

    /**
     * @brief データをパケットに分割してbuffに挿入（sender）
     * @note
     * 上書きにより削除されたデータをinfoから削除
     * nextseqの修正
     */
    void push_packets(unsigned char *data, uint32_t len, uint32_t max_payload_size);

    /**
     * @brief buffにデータを挿入（receiver）
     */
    void insert_packet(unsigned char *data, uint32_t len, uint32_t idx, uint32_t head_idx, uint32_t tail_idx);

    /**
     * @brief buffからデータを削除（sender/receiver）
     */
    uint32_t pop_packets(unsigned char *data);

    /**
     * @brief buffからindexのデータを取得（sender/receiver）
     */
    uint32_t get_packet(uint32_t idx, unsigned char *data, uint32_t *head_idx, uint32_t *tail_idx);
private:
    class packet
    {
    public:
        packet(uint32_t payload_size, uint32_t head_idx, uint32_t tail_idx)
            : payload_size(payload_size), head_idx(head_idx), tail_idx(tail_idx) {}

        uint32_t payload_size;
        uint32_t head_idx;
        uint32_t tail_idx;
    };

    std::mutex mtx;
    ring_buffer buff;
    std::map<uint64_t, packet> info;    // idx : packet
    uint32_t next_idx; // only receiver
};

} // namespace gcom
