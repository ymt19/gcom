#pragma once

#include "ring_buffer.hpp"
#include <mutex>
#include <queue>

namespace gcom
{

class send_stream
{
public:
    send_stream(int buffsize) : nextseq(1), buff(buffsize) {}
private:
    std::mutex mtx;
    int nextseq;
    ring_buffer buff;
    std::queue<queue_entry> info;
};

class recv_stream
{
public:
    recv_stream(int buffsize) : buff(buffsize) {}
private:
    std::mutex mtx;
    ring_buffer buff;
    std::priority_queue<queue_entry, std::vector<queue_entry>, std::greater<queue_entry>> info;
};

class queue_entry
{
public:
    uint64_t idx;
    uint32_t payload_len;
    uint32_t seq;
    uint32_t head;
    uint32_t tail;
    int src_id;    // send bufの場合-1
    int dest_id;   // recv bufの場合-1

    queue_entry(uint64_t idx, uint32_t payload_len, uint64_t seq, uint32_t head, uint32_t tail, int src_id, int dest_id)
        : idx(idx), payload_len(payload_len), seq(seq), head(head), tail(tail), src_id(src_id), dest_id(dest_id) {}

    bool operator< (const queue_entry& a) const { return idx < a.idx; }
    bool operator> (const queue_entry& a) const { return idx > a.idx; }
};

} // namespace gcom
