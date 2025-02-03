#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

size_t align(size_t size, size_t to_align);
size_t next_pow(size_t size);
void *beware_overflow(void *ptr, size_t nmemb, size_t size);
void *page_begin(void *ptr, size_t page_size);

#endif /* ! UTILS_H */
