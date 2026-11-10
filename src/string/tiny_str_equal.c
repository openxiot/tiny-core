/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_str_equal.h>

#include <ctype.h>

TINYCORE_INLINE char tiny_str_equal_fold(char c)
{
    return (char)tolower((unsigned char)c);
}

bool tiny_str_equal(const char *a, const char *b, bool ignore_case)
{
    if (a == b)
    {
        return true;
    }

    if (a == NULL || b == NULL)
    {
        return false;
    }

    if (!ignore_case)
    {
        return strcmp(a, b) == 0;
    }

    while (*a != '\0' && *b != '\0')
    {
        if (tiny_str_equal_fold(*a) != tiny_str_equal_fold(*b))
        {
            return false;
        }

        ++a;
        ++b;
    }

    return *a == *b;
}
