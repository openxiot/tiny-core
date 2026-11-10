/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_SNPRINTF_H
#define TINYCORE_TINY_SNPRINTF_H

#include <tinycore/tiny_platform.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

/*
 * C99 provides snprintf and always NUL terminates on truncation.  Older MSVC
 * runtimes are missing it; their _vsnprintf_s(..., _TRUNCATE, ...) does
 * terminate, so use that instead.
 */
/* The va_list form necessarily forwards a non-literal format; callers are
 * still checked because tiny_snprintf() carries the printf format
 * attribute. */
#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

TINYCORE_INLINE int tiny_vsnprintf(char *buffer, size_t size, const char *format, va_list args)
{
#if defined(_MSC_VER) && _MSC_VER < 1900
    return _vsnprintf_s(buffer, size, _TRUNCATE, format, args);
#else
    return vsnprintf(buffer, size, format, args);
#endif
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

TINYCORE_INLINE int tiny_snprintf(char *buffer, size_t size, const char *format, ...) TINYCORE_FORMAT_PRINTF(3, 4);

TINYCORE_INLINE int tiny_snprintf(char *buffer, size_t size, const char *format, ...)
{
    int result = 0;
    va_list args;

    va_start(args, format);
    result = tiny_vsnprintf(buffer, size, format, args);
    va_end(args);

    return result;
}

#endif /* TINYCORE_TINY_SNPRINTF_H */
