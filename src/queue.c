#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"
#include "utils.h"

struct queue_entry
{
    void *data;
    struct node *prev;
    struct node *next;
};

/**
 * @brief bufferの初期化
 */
void
queue_init(struct queue *buf)
{
    int en;

    buf->head = NULL;
    buf->tail = NULL;
}

/**
 * @brief bufferの解放
 */
void
queue_free(struct queue *buf)
{
    int en;

    while (buf->head != NULL)
    {
        buffer_pop(buf);
    }
}

/**
 * @brief bufferに要素を追加
 */
void
queue_push(struct queue *buf, struct header *hdr, char *data)
{
    int en;
    struct node *new_node;
    
    new_node = malloc(sizeof(node_t));
    if (new_node == NULL)
        handle_error("malloc");

    memcpy(&new_node->hdr, hdr, sizeof(struct header));
    memcpy(&new_node->data, data, hdr->size);

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
}

/**
 * @brief bufferの先頭要素を削除
 */
extern void
queue_pop(struct queue *buf)
{
    int en;
    struct node *pop_node;

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
}

/**
 * @brief bufferからseqのデータを取得
 */
extern int
queue_get(struct queue *buf, int seq, struct header *hdr, char *data)
{
    int en;
    int ret = -1;
    node_t *now;

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

    return ret;
}