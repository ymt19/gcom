#pragma once

#include "ring_buffer.hpp"
#include <mutex>
#include <queue>

class sstream
{
public:
private:
    std::mutex mtx;
    int seq;
    ring_buffer buff;
    std::queue<queue_entry> info;
};

class rstream
{
public:
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

    queue_entry(uint64_t _idx, uint32_t _payload_len, uint64_t _seq, uint32_t _head, uint32_t _tail, int _src_id, int _dest_id)
    {
        idx = _idx;
        payload_len = _payload_len;
        seq = _seq;
        head = _head;
        tail = _tail;
        src_id = _src_id;
        dest_id = _dest_id;
    }

    bool operator< (const queue_entry& a) const { return idx < a.idx; }
    bool operator> (const queue_entry& a) const { return idx > a.idx; }
};