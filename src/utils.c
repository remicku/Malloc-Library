#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

size_t align(size_t size, size_t to_align)
{
    if (size % to_align == 0)
        return size;

    size_t res = 0;

    if (__builtin_add_overflow(size, to_align - (size % to_align), &res))
        return 0;

    return res;
}

size_t next_pow(size_t size)
{
    size_t res = sizeof(long double);

    while (res < size)
    {
        res *= 2;
    }

    return res;
}

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t res;

    if (__builtin_mul_overflow(nmemb, size, &res))
        return NULL;

    char *p = ptr;

    return p + res;
}

void *page_begin(void *ptr, size_t page_size)
{
    size_t address = (size_t)ptr;

    size_t page_start = address & ~(page_size - 1);

    char *p = 0;

    return p + page_start;
}
