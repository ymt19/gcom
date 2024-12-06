#pragma once

#include "ring_buffer.hpp"
#include "endpoint.hpp"
#include <mutex>
#include <queue>

namespace gcom
{

class send_stream
{
public:
    send_stream(endpoint ep, int buffsize) : ep(ep), nextseq(1), buff(buffsize) {}

    // データをパケットに分割して，buffに挿入
    // 上書きにより削除されたデータをinfoから削除
    // nextseqの修正
    void push();

    // buffからデータを削除
    void pop();
private:
    std::mutex mtx;
    endpoint ep;
    int nextseq;
    ring_buffer buff;
    std::queue<queue_entry> info;
};

class recv_stream
{
public:
    recv_stream(endpoint ep, int buffsize) : ep(ep), buff(buffsize) {}

    // buffにデータ挿入
    // 指定されたseqからindexを特定
    void insert();

    /* buffからデータを削除 */
    void pop();

    /* buffからデータ取得 */
    std::string get();
private:
    // 必要ならinfoからデータ削除
    bool allocate();

    std::mutex mtx;
    endpoint ep;
    ring_buffer buff;
    std::priority_queue<queue_entry, std::vector<queue_entry>, std::greater<queue_entry>> info;
};

class queue_entry
{
public:
    queue_entry(uint64_t idx, uint32_t payload_len, uint64_t seq, uint32_t head, uint32_t tail, int src_id, int dest_id)
        : idx(idx), payload_len(payload_len), seq(seq), head(head), tail(tail), src_id(src_id), dest_id(dest_id) {}

    bool operator< (const queue_entry& a) const { return idx < a.idx; }
    bool operator> (const queue_entry& a) const { return idx > a.idx; }

    uint64_t idx;
    uint32_t payload_len;
    uint32_t seq;
    uint32_t head;
    uint32_t tail;
    int src_id;    // send bufの場合-1
    int dest_id;   // recv bufの場合-1
};

} // namespace gcom
