/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_TIME_H
#define TINYCORE_TINY_TIME_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/* Microseconds since the Unix epoch (1 Jan 1970 UTC).
 *
 * On platforms without a real-time clock (ESP-IDF, bare metal) this is the
 * platform's uptime clock instead, so only differences between two values
 * are meaningful there. */
TINYCORE_API uint64_t tiny_current_microsecond(void);

/* Milliseconds since the Unix epoch (see the caveat above). */
TINYCORE_API uint64_t tiny_current_millisecond(void);

/* Writes the local time as "YYYY-MM-DD HH:MM:SS" into buffer. */
TINYCORE_API TinyRet tiny_getstrtime(char *buffer, size_t size);

/* Writes the local time using strftime()'s format. */
TINYCORE_API TinyRet tiny_getstrtime_format(char *buffer, size_t size, const char *format);

/* Suspend the calling thread. */
TINYCORE_API void tiny_sleep_ms(uint32_t milliseconds);
TINYCORE_API void tiny_sleep_us(uint32_t microseconds);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_TIME_H */
