/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_strdup.h>

char *tiny_strdup(const char *str)
{
    size_t length = 0;
    char *copy = NULL;

    if (str == NULL)
    {
        return NULL;
    }

    length = strlen(str) + 1;
    copy = (char *)tiny_malloc(length);
    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, str, length);
    return copy;
}

char *tiny_strndup(const char *str, size_t maxlen)
{
    size_t length = 0;
    char *copy = NULL;

    if (str == NULL)
    {
        return NULL;
    }

    /* memchr() is C89 and available on every supported platform; strnlen()
     * is POSIX.1-2008 and missing from the MSVC runtime. */
    {
        const char *end = (const char *)memchr(str, '\0', maxlen);
        length = (end != NULL) ? (size_t)(end - str) : maxlen;
    }

    copy = (char *)tiny_malloc(length + 1);
    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, str, length);
    copy[length] = '\0';
    return copy;
}
