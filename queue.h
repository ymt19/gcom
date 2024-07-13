#pragma once

#include <pthread.h>

struct queue_entry;

/**
 * @brief queue
 */
struct queue
{
    struct queue_entry *head;
    struct queue_entry *tail;
};

extern void
queue_init(struct queue *buf);
extern void
queue_free(struct queue *buf);
extern void
queue_push(struct queue *buf, char *data);
extern void
queue_pop(struct queue *buf);
extern int
queue_get(struct queue *buf, int seq, char *data);