/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_RANDOM_H
#define TINYCORE_TINY_RANDOM_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/*
 * Small PCG32 pseudo random generator.
 *
 * The generator is seeded automatically from the clock on first use; call
 * tiny_random_seed() to make a sequence reproducible.  It is not
 * cryptographically secure and it is not thread safe: a host that calls it
 * from several threads must serialize access.
 */

TINYCORE_API void tiny_random_seed(uint32_t seed);

TINYCORE_API uint32_t tiny_random_u32(void);

/* Fill buffer with length random bytes. */
TINYCORE_API TINYCORE_RET tiny_random_bytes(uint8_t *buffer, size_t length);

/* Compatibility wrapper for older TinyCore code. */
TINYCORE_API void tiny_random_create(uint8_t *value, uint8_t length);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_RANDOM_H */
