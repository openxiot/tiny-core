/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_PLATFORM_H
#define TINYCORE_TINY_PLATFORM_H

/*
 * Platform detection.
 *
 * Every other TinyCore header includes this file first.  The canonical
 * macros below may also be defined by the build system, which is the
 * recommended way to select a platform when cross compiling.
 */

#if defined(_WIN32) || defined(__WIN32__)
#  if !defined(TINYCORE_PLATFORM_WINDOWS)
#    define TINYCORE_PLATFORM_WINDOWS 1
#  endif
#elif defined(__APPLE__)
#  if !defined(TINYCORE_PLATFORM_APPLE)
#    define TINYCORE_PLATFORM_APPLE 1
#  endif
#elif defined(__ANDROID__)
#  if !defined(TINYCORE_PLATFORM_ANDROID)
#    define TINYCORE_PLATFORM_ANDROID 1
#  endif
#elif defined(__linux__)
#  if !defined(TINYCORE_PLATFORM_LINUX)
#    define TINYCORE_PLATFORM_LINUX 1
#  endif
#endif

/* Legacy platform selects, kept so existing toolchains keep working. */
#if defined(ESP32) && !defined(TINYCORE_PLATFORM_ESP32)
#  define TINYCORE_PLATFORM_ESP32 1
#endif
#if defined(ESP8266) && !defined(TINYCORE_PLATFORM_ESP8266)
#  define TINYCORE_PLATFORM_ESP8266 1
#endif
#if defined(MT7697) && !defined(TINYCORE_PLATFORM_MT7697)
#  define TINYCORE_PLATFORM_MT7697 1
#endif
#if defined(N32G45X) && !defined(TINYCORE_PLATFORM_N32G45X)
#  define TINYCORE_PLATFORM_N32G45X 1
#endif
#if defined(__OPENWRT_MT7688__) && !defined(TINYCORE_PLATFORM_OPENWRT_MT7688)
#  define TINYCORE_PLATFORM_OPENWRT_MT7688 1
#endif

#if defined(TINYCORE_PLATFORM_LINUX) || defined(TINYCORE_PLATFORM_ANDROID) || \
    defined(TINYCORE_PLATFORM_APPLE) || defined(TINYCORE_PLATFORM_OPENWRT_MT7688)
#  define TINYCORE_PLATFORM_POSIX 1
#endif

#if defined(TINYCORE_PLATFORM_POSIX) && !defined(TINYCORE_NO_PTHREAD)
#  define TINYCORE_HAS_PTHREAD 1
#else
#  define TINYCORE_HAS_PTHREAD 0
#endif

/* Compiler detection. */
#if defined(_MSC_VER)
#  define TINYCORE_COMPILER_MSVC 1
#elif defined(__clang__)
#  define TINYCORE_COMPILER_CLANG 1
#elif defined(__GNUC__)
#  define TINYCORE_COMPILER_GCC 1
#endif

/* Inline helper used by headers. */
#if defined(_MSC_VER)
#  define TINYCORE_INLINE static __inline
#elif defined(__GNUC__) || defined(__clang__)
#  define TINYCORE_INLINE static inline __attribute__((unused))
#else
#  define TINYCORE_INLINE static inline
#endif

/* Backwards-compatible spelling. */
#ifndef TINY_INLINE
#  define TINY_INLINE TINYCORE_INLINE
#endif

#if defined(TINYCORE_COMPILER_GCC) || defined(TINYCORE_COMPILER_CLANG)
#  define TINYCORE_FORMAT_PRINTF(fmt_index, args_index) \
    __attribute__((format(printf, fmt_index, args_index)))
#  define TINYCORE_DEPRECATED __attribute__((deprecated))
#else
#  define TINYCORE_FORMAT_PRINTF(fmt_index, args_index)
#  define TINYCORE_DEPRECATED
#endif

#endif /* TINYCORE_TINY_PLATFORM_H */
