#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "buffer.h"

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#ifdef DEBUG
#define DEBUG_PRINT(...) \
    fprintf(stderr, "%s(%d) %s:", __FILE__, __LINE__, __func__), \
    fprintf(stderr, "Debug: %s\n", __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

/**
 * @brief bufferの初期化
 */
extern void
buffer_init(buffer_t *buf)
{
    int en;

    buf->head = NULL;
    buf->tail = NULL;

    en = pthread_mutex_init(&buf->mutex, NULL);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_init");
}

/**
 * @brief bufferの解放
 */
extern void
buffer_free(buffer_t *buf)
{
    int en;

    while (buf->head != NULL)
    {
        buffer_pop(buf);
    }

    en = pthread_mutex_destroy(&buf->mutex);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_destroy");
}

/**
 * @brief bufferに要素を追加
 */
extern void
buffer_push(buffer_t *buf, struct header *hdr, char *data)
{
    int en;
    node_t *new_node;
    
    new_node = malloc(sizeof(node_t));
    if (new_node == NULL)
        handle_error("malloc");

    memcpy(&new_node->hdr, hdr, sizeof(struct header));
    memcpy(&new_node->data, data, hdr->size);

    // バッファを加工
    en = pthread_mutex_lock(&buf->mutex);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_lock");

    // buffが空の場合
    if (buf->head == NULL)
    {
        new_node->prev = NULL;
        new_node->next = NULL;
        buf->head = new_node;
        buf->tail = new_node;
    }
    else
    {
        new_node->prev = buf->tail;
        new_node->next = NULL;
        buf->tail->next = new_node;
        buf->tail = new_node;
    }

    en = pthread_mutex_unlock(&buf->mutex);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_unlock");
}

/**
 * @brief bufferの先頭要素を削除
 */
extern void
buffer_pop(buffer_t *buf)
{
    int en;
    node_t *pop_node;

    // バッファを加工
    en = pthread_mutex_lock(&buf->mutex);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_lock");

    // bufが空ではない場合
    if (buf->head != NULL)
    {
        pop_node = buf->head;
        // buffの要素数が1の場合
        if (buf->head->next == NULL)
        {
            buf->head = NULL;
            buf->tail = NULL;
        }
        // bufの要素数が2以上の場合
        else if (buf->head->next != NULL)
        {
            buf->head = buf->head->next;
            buf->head->prev = NULL;
        }
        free(pop_node);
    }
    
    en = pthread_mutex_unlock(&buf->mutex);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_unlock");
}

/**
 * @brief bufferからseqのデータを取得
 */
extern int
buffer_get(buffer_t *buf, int seq, struct header *hdr, char *data)
{
    int en;
    int ret = -1;
    node_t *now;

    en = pthread_mutex_lock(&buf->mutex);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_lock");

    now = buf->head;
    while (now != NULL)
    {
        if (now->hdr.seq == seq)
        {
            memcpy(hdr, &now->hdr, sizeof(struct header));
            memcpy(data, &now->data, hdr->size);
            ret = 0;
            break;
        }
        now = now->next;
    }

    en = pthread_mutex_unlock(&buf->mutex);
    if (en != 0)
        handle_error_en(en, "pthread_mutex_unlock");

    return ret;
}