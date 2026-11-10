/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_API_H
#define TINYCORE_TINY_API_H

#include <tinycore/tiny_platform.h>

/*
 * TINYCORE_SHARED is defined by the build system for consumers of a shared
 * library.  TINYCORE_EXPORTS is defined while compiling the library itself.
 * When neither is defined (static library or source drop) the API macro is
 * empty.
 *
 * TINYCORE_STATIC may be defined by consumers that link a static library
 * explicitly.
 */
#if defined(TINYCORE_STATIC) || !defined(TINYCORE_SHARED)
#  define TINYCORE_API
#elif defined(_WIN32) || defined(__CYGWIN__)
#  if defined(TINYCORE_EXPORTS)
#    define TINYCORE_API __declspec(dllexport)
#  else
#    define TINYCORE_API __declspec(dllimport)
#  endif
#elif defined(TINYCORE_COMPILER_GCC) || defined(TINYCORE_COMPILER_CLANG)
#  define TINYCORE_API __attribute__((visibility("default")))
#else
#  define TINYCORE_API
#endif

/* Compatibility aliases used by older TinyCore releases. */
#ifndef TINY_API
#  define TINY_API TINYCORE_API
#endif

#ifndef TINY_EXPORT
#  define TINY_EXPORT TINYCORE_API
#endif

#endif /* TINYCORE_TINY_API_H */
