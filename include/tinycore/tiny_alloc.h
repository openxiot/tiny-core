/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_ALLOC_H
#define TINYCORE_TINY_ALLOC_H

#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/*
 * Thin allocation wrappers.  They exist so that a host project can route
 * TinyCore allocations through a custom allocator by providing its own
 * definitions, and so that every allocation is zero checked in one place.
 *
 * TinyCore always calls tiny_free() on memory it owns; never mix these with
 * the C library directly.
 */
TINYCORE_API void *tiny_malloc(size_t size);
TINYCORE_API void *tiny_calloc(size_t count, size_t size);
TINYCORE_API void *tiny_realloc(void *ptr, size_t size);
TINYCORE_API void tiny_free(void *ptr);

/* Duplicate a NUL terminated string.  Returns NULL when str is NULL or on
 * allocation failure. */
TINYCORE_API char *tiny_strdup(const char *str);

/* Duplicate at most maxlen bytes of str and NUL terminate the result. */
TINYCORE_API char *tiny_strndup(const char *str, size_t maxlen);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_ALLOC_H */
