#include "stream.hpp"

void gcom::stream::push_packets(unsigned char *data, int len)
{
    uint64_t head, tail;

    // 分割して挿入
    while(1)
    {
        uint64_t idx;
        idx = buffer.push(data, len);
        info.push(packet(idx, len, nextseq, head, tail));
        nextseq += len;
    }

    // zombie indexを確認して，infoから削除
    if (buffer.get_zombie_idx() > info.top().idx)
    {

    }
}

void gcom::stream::push_empty(int len)
{
    uint64_t idx;
    idx = buffer.push(len);

    // zombie indexを確認して，infoから削除
    if (buffer.get_zombie_idx() > info.top().idx)
    {

    }
}

void gcom::stream::insert_packet(unsigned char *data, int len, uint64_t seq, uint64_t head, uint64_t tail)
{
    uint64_t idx;

    idx = search_idx(seq);
    buffer.set(idx, data, len);
    info.push(packet(idx, len, seq, head, tail));

    // nextseqの計算
}

void gcom::stream::pop_packets()
{

}

void gcom::stream::get_packet()
{

}

uint64_t gcom::stream::search_idx(uint64_t seq)
{

}