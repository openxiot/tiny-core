/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_IEEE754_H
#define TINYCORE_IEEE754_H

#include <tinycore/tiny_base.h>

TINYCORE_BEGIN_DECLS

/*
 * IEEE 754 conversions.
 *
 * The bit level helpers assume the platform uses IEEE 754 binary32/binary64
 * (true on every mainstream target).  The f32 wire helpers use network byte
 * order (most significant byte first), which is what TLV8 and most embedded
 * protocols expect.
 */

TINYCORE_API uint32_t tiny_ieee754_float_to_bits(float value);
TINYCORE_API float tiny_ieee754_bits_to_float(uint32_t bits);

TINYCORE_API uint64_t tiny_ieee754_double_to_bits(double value);
TINYCORE_API double tiny_ieee754_bits_to_double(uint64_t bits);

TINYCORE_API void tiny_ieee754_encode_f32(float value, uint8_t out[4]);
TINYCORE_API float tiny_ieee754_decode_f32(const uint8_t in[4]);

TINYCORE_END_DECLS

#endif /* TINYCORE_IEEE754_H */
