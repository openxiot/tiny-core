/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_STR_EQUAL_H
#define TINYCORE_TINY_STR_EQUAL_H

#include <tinycore/tiny_base.h>

TINYCORE_BEGIN_DECLS

/* Compare two NUL terminated strings.  NULL equals NULL; NULL never equals a
 * non-NULL string.  Case folding is ASCII only. */
TINYCORE_API bool tiny_str_equal(const char *a, const char *b, bool ignore_case);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_STR_EQUAL_H */
