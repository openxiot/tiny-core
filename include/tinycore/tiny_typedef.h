/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_TYPEDEF_H
#define TINYCORE_TINY_TYPEDEF_H

#include <tinycore/tiny_platform.h>
#include <tinycore/tiny_api.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#  define TINYCORE_BEGIN_DECLS extern "C" {
#  define TINYCORE_END_DECLS }
#else
#  define TINYCORE_BEGIN_DECLS
#  define TINYCORE_END_DECLS
#endif

/* Compatibility aliases. */
#ifndef TINY_BEGIN_DECLS
#  define TINY_BEGIN_DECLS TINYCORE_BEGIN_DECLS
#  define TINY_END_DECLS TINYCORE_END_DECLS
#endif

#ifndef STR_EQUAL
#  define STR_EQUAL(a, b) (strcmp((a), (b)) == 0)
#endif

#ifndef DIM
#  define DIM(a) (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef MIN
#  define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#  define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

/* Public API annotations. */
#ifndef TINYCORE_IN
#  define TINYCORE_IN
#endif
#ifndef TINYCORE_OUT
#  define TINYCORE_OUT
#endif
#ifndef TINYCORE_NULLABLE
#  define TINYCORE_NULLABLE
#endif

#define TINYCORE_UNUSED(x) ((void)(x))

/* Argument checking macros are part of the everyday vocabulary of the
 * library, so make them available together with the basic types. */
#include <tinycore/tiny_debug.h>

#endif /* TINYCORE_TINY_TYPEDEF_H */
