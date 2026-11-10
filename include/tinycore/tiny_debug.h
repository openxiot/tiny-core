/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_DEBUG_H
#define TINYCORE_TINY_DEBUG_H

#include <stdio.h>

/*
 * Argument checking is always performed.  TINY_DEBUG only controls whether a
 * diagnostic is printed.  The original implementation compiled the checks
 * away in release builds, which silently turned an argument error into a
 * crash far from the real cause.
 */
#if defined(TINY_DEBUG)
#  define TINYCORE_CHECK_LOG(expr) \
    do { \
        fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, (expr)); \
    } while (0)
#else
#  define TINYCORE_CHECK_LOG(expr) ((void)(expr))
#endif

#define RETURN_IF_FAIL(p) \
    do { if (!(p)) { TINYCORE_CHECK_LOG(#p); return; } } while (0)

#define RETURN_VAL_IF_FAIL(p, ret) \
    do { if (!(p)) { TINYCORE_CHECK_LOG(#p); return (ret); } } while (0)

#define RETURN_IF_FAIL_MSG(p, msg) \
    do { if (!(p)) { TINYCORE_CHECK_LOG(msg); return; } } while (0)

#define RETURN_VAL_IF_FAIL_MSG(p, ret, msg) \
    do { if (!(p)) { TINYCORE_CHECK_LOG(msg); return (ret); } } while (0)

#endif /* TINYCORE_TINY_DEBUG_H */
