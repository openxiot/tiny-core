/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_CHAR_UTIL_H
#define TINYCORE_TINY_CHAR_UTIL_H

#include <tinycore/tiny_base.h>

TINYCORE_BEGIN_DECLS

/* Plain ASCII tests.  Unlike <ctype.h> they are locale independent and never
 * have undefined behaviour for negative values. */

TINYCORE_API bool tiny_is_ascii(int c);
TINYCORE_API bool tiny_is_control(int c);

/* RFC 3261 "tspecial" set: the separators of an HTTP/SIP header value. */
TINYCORE_API bool tiny_is_tspecial(int c);

TINYCORE_API bool tiny_is_digit(int c);
TINYCORE_API bool tiny_is_hex(char c);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_CHAR_UTIL_H */
