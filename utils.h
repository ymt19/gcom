#pragma once

#include <stdio.h>

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