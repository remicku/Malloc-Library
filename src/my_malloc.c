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