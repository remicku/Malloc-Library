#include "my_malloc.h"

#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "bucket.h"
#include "utils.h"

struct bucket_list bl = { .data = NULL };

void *my_malloc(size_t size)
{
    size_t block_size = next_pow(size);
    size_t total_size =
        align(block_size + sizeof(struct bucket), sysconf(_SC_PAGESIZE));

    struct bucket *cur = bl.data;
    struct bucket *prev = cur;

    if (cur == NULL)
    {
        struct bucket *b = bucket_create(block_size, total_size);
        cur = b;
        bl.data = b;
    }
    else
    {
        while (cur != NULL
               && (block_size != cur->block_size
                   || cur->block_used == cur->capacity))
        {
            prev = cur;
            cur = cur->next;
        }

        if (cur == NULL)
        {
            struct bucket *b = bucket_create(block_size, total_size);
            prev->next = b;
            cur = b;
        }
    }

    void *block = bucket_allocate(cur);

    return block;
}

void my_free(void *ptr)
{
    if (ptr == NULL)
        return;

    void *p = page_begin(ptr, sysconf(_SC_PAGESIZE));
    struct bucket *b = bl.data;
    struct bucket *prev = b;

    while (b != NULL && b != p)
    {
        prev = b;
        b = b->next;
    }

    if (b == NULL)
        return;

    void *chunk_start = b->chunk;
    char *chunk_end = (char *)chunk_start + (b->block_size * b->capacity);

    if (ptr < chunk_start || ptr >= (void *)chunk_end)
        return;

    block_free(b, ptr);

    if (b->block_used == 0)
    {
        if (b == bl.data)
        {
            prev = b->next;
            bl.data = prev;
        }
        else
        {
            prev->next = b->next;
        }

        bucket_destroy(b);
    }
}

void *my_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return my_malloc(size);

    if (size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    void *page = page_begin(ptr, sysconf(_SC_PAGESIZE));

    struct bucket *b = bl.data;

    while (b != NULL && b != page)
        b = b->next;

    if (b == NULL)
    {
        return NULL;
    }

    if (size <= b->block_size)
        return ptr;

    void *p = my_malloc(size);

    if (p == NULL)
        return NULL;

    memcpy(p, ptr, b->block_size);

    my_free(ptr);

    return p;
}

void *my_calloc(size_t n, size_t size)
{
    size_t res;

    if (__builtin_mul_overflow(n, size, &res))
        return NULL;

    char *arr = my_malloc(size * n);

    if (arr == NULL)
        return NULL;

    for (size_t i = 0; i < n * size; i++)
    {
        arr[i] = 0;
    }

    return arr;
}
