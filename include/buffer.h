#pragma once

#include <pthread.h>

#include "header.h"

struct node_t
{
    struct header hdr;
    char data[DATA_SIZE_MAX];
    struct node_t *prev;
    struct node_t *next;
};
typedef struct node_t node_t;

/**
 * @brief 優先度付きキュー <= ただの双方向リスト
 */
struct buffer_t
{
    node_t *head;
    node_t *tail;
    pthread_mutex_t mutex;
};
typedef struct buffer_t buffer_t;

void buffer_init(buffer_t *buf);
void buffer_free(buffer_t *buf);
void buffer_push(buffer_t *buf, struct header *hdr, char *data);
void buffer_pop(buffer_t *buf);
int buffer_get(buffer_t *buf, int seq, struct header *hdr, char *data);