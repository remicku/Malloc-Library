#include "bucket.h"

#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

void bucket_free(struct bucket *b)
{
    munmap(b, (b->block_size * b->capacity) + sizeof(struct bucket));
}

struct bucket *bucket_create(size_t block_size, size_t total_size)
{
    if (block_size == 0 || total_size == 0)
        return NULL;

    struct bucket *b =
        mmap(NULL, align(total_size, sysconf(_SC_PAGESIZE)),
             PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (b == MAP_FAILED)
        return NULL;

    b->size = total_size;
    b->block_size = block_size;
    b->capacity =
        (total_size - align(sizeof(struct bucket), sizeof(long double)))
        / block_size;
    b->chunk = (void *)align((size_t)(b + 1), sizeof(long double));

    if (b->chunk == NULL)
        return NULL;

    b->free = b->chunk;

    struct free_list *l = b->free;

    for (size_t i = 1; i < b->capacity; i++)
    {
        l->next = l + (block_size / sizeof(size_t));
        l = l->next;
    }

    l->next = NULL;
    b->next = NULL;
    b->block_used = 0;

    return b;
}

void *bucket_allocate(struct bucket *b)
{
    if (b == NULL || b->free == NULL)
        return NULL;

    void *block = b->free;
    struct free_list *n = b->free;
    b->free = n->next;
    b->block_used += 1;

    return block;
}

void block_free(struct bucket *b, void *block)
{
    if (b == NULL || block == NULL)
        return;

    struct free_list *fb = block;
    fb->next = b->free;
    b->free = block;
    b->block_used -= 1;
}

void bucket_destroy(struct bucket *b)
{
    if (b == NULL)
        return;

    munmap(b, b->size);
}
