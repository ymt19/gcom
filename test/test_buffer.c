#include <stdio.h>
#include <string.h>

#include "../buffer.h"

#define BUF_SIZE    16

int main()
{
    struct buffer buf;
    uint64_t idx;
    char str0[32];
    char str1[32] = "aaaaaa";       // len: 6
    char str2[32] = "bbbbbb";       // len: 6
    char str3[32] = "cccccc";       // len:6
    // char str4[32] = "ddddddddd";    // len:9
    // uint64_t idx = 0;

    buffer_init(&buf, BUF_SIZE);

    fprintf(stdout, "   write_idx:%ld, read_idx:%ld\n", buf.write_idx, buf.read_idx);

    fprintf(stdout, "\n");

    idx = buf.write_idx;
    buffer_push(&buf, (uint8_t *)str1, strlen(str1));
    fprintf(stdout, "   write_idx:%ld, read_idx:%ld\n", buf.write_idx, buf.read_idx);
    buffer_get(&buf, idx, (uint8_t *)str0, strlen(str1));
    str0[strlen(str1)] = '\0';
    fprintf(stdout, "%s:%s\n", str1, str0);
    buffer_pop(&buf, strlen(str1));

    fprintf(stdout, "\n");

    idx = buf.write_idx;
    buffer_push(&buf, (uint8_t *)str2, strlen(str2));
    fprintf(stdout, "   write_idx:%ld, read_idx:%ld\n", buf.write_idx, buf.read_idx);
    buffer_get(&buf, idx, (uint8_t *)str0, strlen(str2));
    str0[strlen(str2)] = '\0';
    fprintf(stdout, "%s:%s\n", str2, str0);

    fprintf(stdout, "\n");

    idx = buf.write_idx;
    buffer_push(&buf, (uint8_t *)str3, strlen(str3));
    fprintf(stdout, "   write_idx:%ld, read_idx:%ld\n", buf.write_idx, buf.read_idx);
    buffer_get(&buf, idx, (uint8_t *)str0, strlen(str3));
    str0[strlen(str3)] = '\0';
    fprintf(stdout, "%s:%s\n", str3, str0);

    buffer_free(&buf);
}