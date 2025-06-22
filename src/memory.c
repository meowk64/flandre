#include "memory.h"

#include <stddef.h>
#include <stdlib.h>

void * fln_allocate(size_t size)
{
    return malloc(size);
}

void * fln_allocate_aligned(size_t size, size_t alignment)
{
    return aligned_alloc(alignment, size);
}

void * fln_calloc(size_t count, size_t size)
{
    return calloc(count, size);
}

void * fln_realloc(void * ptr, size_t size)
{
    return realloc(ptr, size);
}

void fln_free(void * ptr)
{
    free(ptr);
}

