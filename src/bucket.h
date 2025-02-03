#ifndef BUCKET_H
#define BUCKET_H

#include <stddef.h>

struct bucket
{
    size_t size;
    size_t block_size;
    size_t capacity; // number of blocks in the chunk
    void *chunk; // memory chunk containing all blocks
    void *free; // address of the first free block of the free list
    size_t block_used;
    struct bucket *next;
};

struct free_list
{
    struct free_list *next; // next free block
};

struct bucket_list
{
    struct bucket *data;
};

void bucket_free(struct bucket *b);
struct bucket *bucket_create(size_t block_size, size_t total_size);
void *bucket_allocate(struct bucket *b);
void block_free(struct bucket *b, void *block);
void bucket_destroy(struct bucket *b);

#endif /* ! BUCKET_H */
