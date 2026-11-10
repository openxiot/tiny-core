/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <string.h>

#include <tinycore/tiny_random.h>

/*
 * Independent PCG32 implementation used as the golden reference.  It repeats
 * the seeding and output function documented by O'Neill so the test pins the
 * generator stream rather than only its reproducibility.
 */
#define REF_MULTIPLIER 6364136223846793005ULL
#define REF_INIT_STATE 0x853c49e6748fea9bULL

static uint64_t ref_state = 0;
static uint64_t ref_inc = 0;

static uint32_t ref_next(void)
{
    uint64_t old_state = ref_state;
    uint32_t xorshifted = (uint32_t)(((old_state >> 18U) ^ old_state) >> 27U);
    uint32_t rotation = (uint32_t)(old_state >> 59U);

    ref_state = old_state * REF_MULTIPLIER + ref_inc;

    return (xorshifted >> rotation) | (xorshifted << ((32U - rotation) & 31U));
}

static void ref_seed(uint32_t seed)
{
    ref_state = 0;
    ref_inc = ((uint64_t)seed << 1U) | 1U;

    (void)ref_next();
    ref_state += REF_INIT_STATE;
    (void)ref_next();
}

static void test_deterministic_sequence(void)
{
    uint8_t a[32];
    uint8_t b[32];
    uint32_t first = 0;
    uint32_t second = 0;
    uint8_t legacy[16];

    tiny_random_seed(1234);
    TINY_TEST_CHECK(tiny_random_bytes(a, sizeof(a)) == TINY_RET_OK);
    tiny_random_seed(1234);
    TINY_TEST_CHECK(tiny_random_bytes(b, sizeof(b)) == TINY_RET_OK);
    TINY_TEST_CHECK(memcmp(a, b, sizeof(a)) == 0);

    tiny_random_seed(1234);
    first = tiny_random_u32();
    second = tiny_random_u32();
    TINY_TEST_CHECK(first != second);

    tiny_random_seed(4321);
    TINY_TEST_CHECK(tiny_random_u32() != first);

    memset(legacy, 0, sizeof(legacy));
    tiny_random_create(legacy, (uint8_t)sizeof(legacy));
    TINY_TEST_CHECK(memcmp(legacy, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", sizeof(legacy)) != 0);
}

static void test_argument_checks(void)
{
    TINY_TEST_CHECK(tiny_random_bytes(NULL, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(tiny_random_bytes(NULL, 4) == TINY_RET_E_NULL_PTR);

    /* The legacy entry point tolerates an empty request. */
    tiny_random_create(NULL, 0);
}

static void test_pcg32_stream(void)
{
    uint32_t expected = 0;
    uint8_t expected_bytes[8];
    uint8_t bytes[8];
    size_t i = 0;

    tiny_random_seed(42);
    ref_seed(42);

    for (i = 0; i < 8; ++i)
    {
        uint32_t actual = 0;

        expected = ref_next();
        actual = tiny_random_u32();
        TINY_TEST_CHECK_MSG(actual == expected,
                            "sample %u: got %u, expected %u",
                            (unsigned)i, (unsigned)actual, (unsigned)expected);
    }

    /* Byte output is the little endian expansion of the same stream. */
    tiny_random_seed(7);
    ref_seed(7);
    for (i = 0; i < 2; ++i)
    {
        uint32_t word = ref_next();
        size_t b = 0;

        for (b = 0; b < 4; ++b)
        {
            expected_bytes[i * 4U + b] = (uint8_t)(word >> (8U * (uint32_t)b));
        }
    }

    TINY_TEST_CHECK(tiny_random_bytes(bytes, sizeof(bytes)) == TINY_RET_OK);
    TINY_TEST_CHECK(memcmp(bytes, expected_bytes, sizeof(bytes)) == 0);

    /* A request that is not a multiple of four spans two words. */
    tiny_random_seed(7);
    ref_seed(7);
    {
        uint32_t first = ref_next();
        uint32_t second = ref_next();

        expected_bytes[0] = (uint8_t)first;
        expected_bytes[1] = (uint8_t)(first >> 8);
        expected_bytes[2] = (uint8_t)(first >> 16);
        expected_bytes[3] = (uint8_t)(first >> 24);
        expected_bytes[4] = (uint8_t)second;
        expected_bytes[5] = (uint8_t)(second >> 8);
    }
    TINY_TEST_CHECK(tiny_random_bytes(bytes, 6) == TINY_RET_OK);
    TINY_TEST_CHECK(memcmp(bytes, expected_bytes, 6) == 0);
}

int main(void)
{
    TINY_TEST_BEGIN("random");
    test_deterministic_sequence();
    test_argument_checks();
    test_pcg32_stream();
    return TINY_TEST_FINISH();
}
