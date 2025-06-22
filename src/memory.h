#pragma once

#include <stddef.h>
#include <stdlib.h>

void * fln_allocate(size_t size);
void * fln_allocate_aligned(size_t size, size_t alignment);
void * fln_calloc(size_t count, size_t size);
void * fln_realloc(void * ptr, size_t size);
void fln_free(void * ptr);
