/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/ieee754.h>

/* memcpy preserves the object representation without violating strict
 * aliasing, which a pointer cast would. */
typedef char tiny_ieee754_float_must_be_4_bytes[(sizeof(float) == 4U) ? 1 : -1];
typedef char tiny_ieee754_double_must_be_8_bytes[(sizeof(double) == 8U) ? 1 : -1];

uint32_t tiny_ieee754_float_to_bits(float value)
{
    uint32_t bits = 0;

    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

float tiny_ieee754_bits_to_float(uint32_t bits)
{
    float value = 0.0f;

    memcpy(&value, &bits, sizeof(value));
    return value;
}

uint64_t tiny_ieee754_double_to_bits(double value)
{
    uint64_t bits = 0;

    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

double tiny_ieee754_bits_to_double(uint64_t bits)
{
    double value = 0.0;

    memcpy(&value, &bits, sizeof(value));
    return value;
}

void tiny_ieee754_encode_f32(float value, uint8_t out[4])
{
    uint32_t bits = tiny_ieee754_float_to_bits(value);

    out[0] = (uint8_t)(bits >> 24);
    out[1] = (uint8_t)(bits >> 16);
    out[2] = (uint8_t)(bits >> 8);
    out[3] = (uint8_t)bits;
}

float tiny_ieee754_decode_f32(const uint8_t in[4])
{
    uint32_t bits = ((uint32_t)in[0] << 24) |
                    ((uint32_t)in[1] << 16) |
                    ((uint32_t)in[2] << 8) |
                    (uint32_t)in[3];

    return tiny_ieee754_bits_to_float(bits);
}
