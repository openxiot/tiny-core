/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_strnlen.h>

size_t tiny_strnlen(const char *s, size_t maxlen)
{
    const char *end = NULL;

    if (s == NULL)
    {
        return 0;
    }

    end = (const char *)memchr(s, '\0', maxlen);

    return (end != NULL) ? (size_t)(end - s) : maxlen;
}
