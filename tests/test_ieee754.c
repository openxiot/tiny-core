/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <math.h>

#include <tinycore/ieee754.h>

static void test_f32_wire_format(void)
{
    uint8_t out[4];
    uint8_t pi[4] = { 0x40, 0x49, 0x0E, 0x99 };
    uint8_t tenth[4] = { 0x3D, 0xCC, 0xCC, 0xCC };

    tiny_ieee754_encode_f32(100.0f, out);
    TINY_TEST_CHECK(out[0] == 0x42 && out[1] == 0xC8 && out[2] == 0x00 && out[3] == 0x00);

    tiny_ieee754_encode_f32(0.1f, out);
    TINY_TEST_CHECK(out[0] == 0x3D && out[1] == 0xCC && out[2] == 0xCC && out[3] == 0xCD);

    {
        float value = tiny_ieee754_decode_f32(pi);
        TINY_TEST_CHECK(value > 3.141515f && value < 3.141517f);
    }

    {
        float value = tiny_ieee754_decode_f32(tenth);
        TINY_TEST_CHECK(value > 0.0999999f && value < 0.1000001f);
    }

    tiny_ieee754_encode_f32(0.0f, out);
    TINY_TEST_CHECK(tiny_ieee754_decode_f32(out) == 0.0f);

    tiny_ieee754_encode_f32(-12.5f, out);
    TINY_TEST_CHECK(tiny_ieee754_decode_f32(out) == -12.5f);
}

static void test_round_trip(void)
{
    static const float floats[] = { 0.0f, 1.0f, -1.0f, 0.1f, 12345.678f, 1e-30f, 1e30f };
    static const double doubles[] = { 0.0, 1.0, -1.0, 0.1, 12345.678, 1e-300, 1e300 };
    size_t i = 0;

    for (i = 0; i < sizeof(floats) / sizeof(floats[0]); ++i)
    {
        TINY_TEST_CHECK(tiny_ieee754_bits_to_float(tiny_ieee754_float_to_bits(floats[i])) == floats[i]);
    }

    for (i = 0; i < sizeof(doubles) / sizeof(doubles[0]); ++i)
    {
        TINY_TEST_CHECK(tiny_ieee754_bits_to_double(tiny_ieee754_double_to_bits(doubles[i])) == doubles[i]);
    }

    TINY_TEST_CHECK(tiny_ieee754_float_to_bits(1.0f) == 0x3F800000U);
    TINY_TEST_CHECK(tiny_ieee754_double_to_bits(1.0) == 0x3FF0000000000000ULL);
}

static void test_special_values(void)
{
    uint8_t out[4];
    float nan_value = 0.0f;
    float inf_value = 0.0f;
    double nan_double = 0.0;
    double inf_double = 0.0;
    uint32_t inf_bits = 0x7F800000U;
    uint64_t inf_bits_double = 0x7FF0000000000000ULL;

    /* Negative zero keeps its sign bit. */
    TINY_TEST_CHECK(tiny_ieee754_float_to_bits(-0.0f) == 0x80000000U);
    TINY_TEST_CHECK(tiny_ieee754_bits_to_float(0x80000000U) == 0.0f);
    TINY_TEST_CHECK(signbit(tiny_ieee754_bits_to_float(0x80000000U)));
    TINY_TEST_CHECK(tiny_ieee754_double_to_bits(-0.0) == 0x8000000000000000ULL);
    TINY_TEST_CHECK(signbit(tiny_ieee754_bits_to_double(0x8000000000000000ULL)));

    /* Positive and negative infinity. */
    TINY_TEST_CHECK(tiny_ieee754_float_to_bits(INFINITY) == inf_bits);
    inf_value = tiny_ieee754_bits_to_float(inf_bits);
    TINY_TEST_CHECK(isinf(inf_value) && inf_value > 0.0f);
    TINY_TEST_CHECK(isinf(tiny_ieee754_bits_to_float(0xFF800000U)));
    TINY_TEST_CHECK(tiny_ieee754_bits_to_float(0xFF800000U) < 0.0f);

    TINY_TEST_CHECK(tiny_ieee754_double_to_bits(INFINITY) == inf_bits_double);
    inf_double = tiny_ieee754_bits_to_double(inf_bits_double);
    TINY_TEST_CHECK(isinf(inf_double) && inf_double > 0.0);

    /* NaN has an all ones exponent and a non zero mantissa. */
    TINY_TEST_CHECK((tiny_ieee754_float_to_bits(NAN) & 0x7F800000U) == 0x7F800000U);
    TINY_TEST_CHECK((tiny_ieee754_float_to_bits(NAN) & 0x007FFFFFU) != 0U);
    nan_value = tiny_ieee754_bits_to_float(0x7FC00000U);
    TINY_TEST_CHECK(isnan(nan_value));

    TINY_TEST_CHECK((tiny_ieee754_double_to_bits(NAN) & 0x7FF0000000000000ULL) ==
                    0x7FF0000000000000ULL);
    TINY_TEST_CHECK((tiny_ieee754_double_to_bits(NAN) & 0x000FFFFFFFFFFFFFULL) != 0ULL);
    nan_double = tiny_ieee754_bits_to_double(0x7FF8000000000000ULL);
    TINY_TEST_CHECK(isnan(nan_double));

    /* The wire format preserves the same special values. */
    tiny_ieee754_encode_f32(INFINITY, out);
    TINY_TEST_CHECK(out[0] == 0x7F && out[1] == 0x80 && out[2] == 0x00 && out[3] == 0x00);
    TINY_TEST_CHECK(isinf(tiny_ieee754_decode_f32(out)));

    tiny_ieee754_encode_f32(NAN, out);
    TINY_TEST_CHECK((out[0] & 0x7FU) == 0x7FU && (out[1] & 0x80U) == 0x80U);
    TINY_TEST_CHECK(isnan(tiny_ieee754_decode_f32(out)));
    TINY_TEST_CHECK(tiny_ieee754_decode_f32(out) != tiny_ieee754_decode_f32(out));
}

int main(void)
{
    TINY_TEST_BEGIN("ieee754");
    test_f32_wire_format();
    test_round_trip();
    test_special_values();
    return TINY_TEST_FINISH();
}
