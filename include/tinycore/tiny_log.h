/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_LOG_H
#define TINYCORE_TINY_LOG_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

#include <stdarg.h>

TINYCORE_BEGIN_DECLS

typedef enum
{
    TINY_LOG_LEVEL_UNKNOWN = 0,
    TINY_LOG_LEVEL_VERBOSE = 1,
    TINY_LOG_LEVEL_DEBUG   = 2,
    TINY_LOG_LEVEL_INFO    = 3,
    TINY_LOG_LEVEL_WARN    = 4,
    TINY_LOG_LEVEL_ERROR   = 5
} TinyLogLevel;

/* Log to the console.  tag may be NULL. */
TINYCORE_API TinyRet tiny_log_init(const char *tag, TinyLogLevel level);

/* Append to a file.  The file is created when missing. */
TINYCORE_API TinyRet tiny_log_init_file(const char *tag, TinyLogLevel level, const char *path);

TINYCORE_API void tiny_log_close(void);
TINYCORE_API void tiny_log_set_level(TinyLogLevel level);
TINYCORE_API TinyLogLevel tiny_log_get_level(void);

/* Write one formatted record.  Prefer the TINY_LOG_* macros below. */
TINYCORE_API void tiny_log_write(TinyLogLevel level,
                                 const char *file,
                                 int line,
                                 const char *format,
                                 ...) TINYCORE_FORMAT_PRINTF(4, 5);

TINYCORE_API void tiny_log_vwrite(TinyLogLevel level,
                                  const char *file,
                                  int line,
                                  const char *format,
                                  va_list args);

/* Dump data as a hex + ASCII table through the log. */
TINYCORE_API void tiny_print_binary(const void *data, size_t size);

#define TINY_LOG_VERBOSE(...) tiny_log_write(TINY_LOG_LEVEL_VERBOSE, __FILE__, __LINE__, __VA_ARGS__)
#define TINY_LOG_DEBUG(...)   tiny_log_write(TINY_LOG_LEVEL_DEBUG,   __FILE__, __LINE__, __VA_ARGS__)
#define TINY_LOG_INFO(...)    tiny_log_write(TINY_LOG_LEVEL_INFO,    __FILE__, __LINE__, __VA_ARGS__)
#define TINY_LOG_WARN(...)    tiny_log_write(TINY_LOG_LEVEL_WARN,    __FILE__, __LINE__, __VA_ARGS__)
#define TINY_LOG_ERROR(...)   tiny_log_write(TINY_LOG_LEVEL_ERROR,   __FILE__, __LINE__, __VA_ARGS__)

/* Compatibility aliases used by older TinyCore releases. */
#ifndef LOG_VERBOSE
#  define LOG_VERBOSE(...) TINY_LOG_VERBOSE(__VA_ARGS__)
#  define LOG_DEBUG(...)   TINY_LOG_DEBUG(__VA_ARGS__)
#  define LOG_INFO(...)    TINY_LOG_INFO(__VA_ARGS__)
#  define LOG_WARN(...)    TINY_LOG_WARN(__VA_ARGS__)
#  define LOG_ERROR(...)   TINY_LOG_ERROR(__VA_ARGS__)
#endif

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_LOG_H */
