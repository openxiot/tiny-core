/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_STRNLEN_H
#define TINYCORE_TINY_STRNLEN_H

#include <tinycore/tiny_base.h>

TINYCORE_BEGIN_DECLS

/* Portable strlen() bounded by maxlen: never reads more than maxlen bytes.
 * Returns 0 for NULL. */
TINYCORE_API size_t tiny_strnlen(const char *s, size_t maxlen);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_STRNLEN_H */
