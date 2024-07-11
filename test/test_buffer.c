#include <stdio.h>
#include <string.h>

#include "buffer.h"

int main()
{
    buffer_t buf;
    struct header hdr1, hdr2;
    char data1[DATA_SIZE_MAX], data2[DATA_SIZE_MAX];

    buffer_init(&buf);

    strcpy(data1, "abcdefg\0");
    hdr1.seq = 1;
    hdr1.first = 1;
    hdr1.last = 1;
    hdr1.size = strlen(data1);
    buffer_push(&buf, &hdr1, data1);
    buffer_get(&buf, 1, &hdr2, data2);
    fprintf(stdout, "push...seq:%d,first:%d,last:%d,size:%d,data:%s\n", hdr1.seq, hdr1.first, hdr1.last, hdr1.size, data1);
    fprintf(stdout, "get....seq:%d,first:%d,last:%d,size:%d,data:%s\n", hdr2.seq, hdr2.first, hdr2.last, hdr2.size, data2);

    strcpy(data2, "ijkl\0");
    hdr1.seq = 2;
    hdr1.first = 2;
    hdr1.last = 2;
    hdr1.size = strlen(data1);
    buffer_push(&buf, &hdr1, data1);
    fprintf(stdout, "%d\n", __LINE__);
    buffer_get(&buf, 2, &hdr2, data2);
    fprintf(stdout, "push...seq:%d,first:%d,last:%d,size:%d,data:%s\n", hdr1.seq, hdr1.first, hdr1.last, hdr1.size, data1);
    fprintf(stdout, "get....seq:%d,first:%d,last:%d,size:%d,data:%s\n", hdr2.seq, hdr2.first, hdr2.last, hdr2.size, data2);

    buffer_pop(&buf);

    buffer_free(&buf);
}