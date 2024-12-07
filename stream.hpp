#pragma once

#include "ring_buffer.hpp"
#include "endpoint.hpp"
#include <mutex>
#include <queue>

namespace gcom
{

class stream
{
public:
    stream(int buffsize, int payload_size)
        : nextseq(0), buffer(buffsize), payload_size(payload_size) {}

    /**
     * @brief データをパケットに分割してbuffに挿入（sender）
     * @note
     * 上書きにより削除されたデータをinfoから削除
     * nextseqの修正
     */
    void gcom::stream::push_packets(unsigned char *data, int len);

    /**
     * @brief buffの指定サイズ分の領域を確保（receiver）
     */
    void push_empty(int len);

    /**
     * @brief buffの指定indexにデータを挿入（receiver）
     */
    void insert_packet(unsigned char *data, int len, uint64_t seq, uint64_t head, uint64_t tail);

    /**
     * @brief buffからデータを削除（sender/receiver）
     */
    void pop_packets();

    /**
     * @brief buffからindexのデータを取得（sender/receiver）
     */
    void get_packet();

    /**
     * @brief seqからidxを検索（sender/receiver）
     */
    uint64_t search_idx(uint64_t seq);
private:
    class packet
    {
    public:
        packet(uint64_t seq, uint64_t head, uint64_t tail, uint64_t idx, uint32_t len)
            : seq(seq), head(head), tail(tail), idx(idx), len(len) {}

        bool operator< (const packet& a) const { return seq < a.seq; }
        bool operator> (const packet& a) const { return seq > a.seq; }

        uint64_t seq;
        uint64_t head;
        uint64_t tail;
        uint64_t idx;
        uint32_t len;
    };

    const int payload_size;
    std::mutex mtx;
    int nextseq;
    ring_buffer buffer;
    std::priority_queue<packet, std::vector<packet>, std::greater<packet>> info;
};

} // namespace gcom
