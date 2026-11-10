/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_alloc.h>

void *tiny_malloc(size_t size)
{
    if (size == 0)
    {
        size = 1;
    }

    return malloc(size);
}

void *tiny_calloc(size_t count, size_t size)
{
    if (count == 0 || size == 0)
    {
        count = 1;
        size = 1;
    }

    return calloc(count, size);
}

void *tiny_realloc(void *ptr, size_t size)
{
    if (size == 0)
    {
        tiny_free(ptr);
        return NULL;
    }

    return realloc(ptr, size);
}

void tiny_free(void *ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}
