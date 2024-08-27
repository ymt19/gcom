#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "utils.h"

/**
 * @brief
 */
void
buffer_init(struct buffer *buf, size_t size)
{
    buf->data = malloc(sizeof(uint8_t) * size);
    if (buf->data == NULL)
        handle_error("malloc");

    buf->size = size;
    buf->write_idx = 0;
    buf->read_idx = 0;
}

/**
 * @brief
 */
void
buffer_free(struct buffer *buf)
{
    free(buf->data);
}


/**
 * @brief
 * @return
 */
int
buffer_push(struct buffer *buf, uint8_t *push, size_t len)
{
    uint64_t remain;
    uint64_t start, end;

    if (buf->data == NULL)
        return -1;

    remain = buf->size - (buf->write_idx - buf->read_idx);
    if (len > remain)
        return -1;

    start = buf->write_idx;
    end = start + len;

    // データを挿入する
    if (start / buf->size != end / buf->size)
    {// 分離する必要がある場合
        memcpy(buf->data + (start % buf->size), push, len - end % buf->size);
        memcpy(buf->data, push + (len - end % buf->size), end % buf->size);
    }
    else
    {
        memcpy(buf->data + start % buf->size, push, len);
    }

    buf->write_idx += len;

    return 0;
}

/**
 * @brief
 * @details
 */
int
buffer_push_empty(struct buffer *buf, size_t len)
{
    uint64_t remain;

    if (buf->data == NULL)
        return -1;

    remain = buf->size - (buf->write_idx - buf->read_idx);
    if (len > remain)
        return -1;

    buf->write_idx += len;

    return 0;
}

/**
 * @brief
 * @details
 * 
 */
int
buffer_pop(struct buffer *buf, size_t len)
{
    if (buf->data == NULL)
        return -1;

    if (buf->write_idx > buf->read_idx + len)
        return -1;
    
    buf->read_idx += len;
    return 0;
}

/**
 * @brief 指定したインデックスにデータを挿入
 * @details
 * read_idxからwrite_idxの範囲内にのみ挿入できる
 * 
 */
int
buffer_set(struct buffer *buf, uint64_t idx, uint8_t *set, size_t len)
{
    uint64_t min, max;
    uint64_t start, end;

    if (buf->data == NULL)
        return -1;

    min = buf->read_idx;
    max = buf->write_idx - len;
    if (idx < min || idx > max)
        return -1;

    start = idx;
    end = idx + len;

    // データを挿入する
    if (start / buf->size != end / buf->size)
    {// リングバッファの先頭末尾で分離する場合
        memcpy(buf->data + (start % buf->size), set, len - end % buf->size);
        memcpy(buf->data, set + (len - end % buf->size), end % buf->size);
    }
    else
    {
        memcpy(buf->data + start % buf->size, set, len);
    }

    return 0;
}

static uint64_t
calc_min_valid_idx(struct buffer *buf)
{
    uint64_t min;

    if (buf->read_idx / buf->size == buf->write_idx / buf->size)
    {
        if (buf->read_idx / buf->size == 0)
            min = 0;
        else
            min = ((buf->read_idx / buf->size) * buf->read_idx) 
                    - (buf->size - buf->write_idx % buf->size);
    }
    else
    {
        min = ((buf->read_idx / buf->size) * buf->read_idx) 
                + (buf->size - buf->write_idx % buf->size);
    }
}

static uint64_t
calc_max_valid_idx(struct buffer *buf)
{
    return buf->write_idx;
}

/**
 * @brief 指定したインデックスのデータを取得
 * @details
 * popしたデータであっても，バッファに残っているなら取得できる
 */
int
buffer_get(struct buffer *buf, uint64_t idx, uint8_t *get, size_t len)
{
    uint64_t min, max;
    uint64_t start, end;

    if (buf->data == NULL)
        return -1;

    min = calc_min_valid_idx(buf);
    max = calc_max_valid_idx(buf) - len;
    if (idx < min || idx > max)
        return -1;

    start = idx;
    end = idx + len;

    // データを取得する
    if (start / buf->size != end / buf->size)
    {// リングバッファの先頭末尾で分離する場合
        memcpy(get, buf->data + (start % buf->size), len - end % buf->size);
        memcpy(get + (len - end % buf->size), buf->data, end % buf->size);
    }
    else
    {
        memcpy(get, buf->data + start % buf->size, len);
    }

    return 0;
}