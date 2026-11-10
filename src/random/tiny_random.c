/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_random.h>
#include <tinycore/tiny_time.h>

/* PCG32 (O'Neill, 2014) constants. */
#define TINY_RANDOM_MULTIPLIER 6364136223846793005ULL
#define TINY_RANDOM_INIT_STATE 0x853c49e6748fea9bULL
#define TINY_RANDOM_INIT_INC   0xda3e39cb94b95bdbULL

static uint64_t tiny_random_state = TINY_RANDOM_INIT_STATE;
static uint64_t tiny_random_inc = TINY_RANDOM_INIT_INC;
static bool tiny_random_seeded = false;

static uint32_t tiny_random_next(void)
{
    uint64_t old_state = tiny_random_state;
    uint32_t xorshifted = 0;
    uint32_t rotation = 0;

    tiny_random_state = old_state * TINY_RANDOM_MULTIPLIER + tiny_random_inc;

    xorshifted = (uint32_t)(((old_state >> 18U) ^ old_state) >> 27U);
    rotation = (uint32_t)(old_state >> 59U);

    return (xorshifted >> rotation) | (xorshifted << ((32U - rotation) & 31U));
}

static void tiny_random_reseed(uint64_t seed)
{
    tiny_random_state = 0U;
    tiny_random_inc = (seed << 1U) | 1U;

    (void)tiny_random_next();
    tiny_random_state += TINY_RANDOM_INIT_STATE;
    (void)tiny_random_next();

    tiny_random_seeded = true;
}

void tiny_random_seed(uint32_t seed)
{
    tiny_random_reseed(seed);
}

uint32_t tiny_random_u32(void)
{
    if (!tiny_random_seeded)
    {
        uint64_t now = tiny_current_microsecond();

        tiny_random_reseed(now ^ (now >> 32));
    }

    return tiny_random_next();
}

TINYCORE_RET tiny_random_bytes(uint8_t *buffer, size_t length)
{
    size_t offset = 0;

    if (buffer == NULL && length > 0)
    {
        return TINY_RET_E_NULL_PTR;
    }

    while (offset < length)
    {
        uint32_t value = tiny_random_u32();
        size_t remaining = length - offset;
        size_t chunk = MIN(remaining, sizeof(value));
        size_t i = 0;

        for (i = 0; i < chunk; ++i)
        {
            buffer[offset + i] = (uint8_t)(value >> (8U * (uint32_t)i));
        }

        offset += chunk;
    }

    return TINY_RET_OK;
}

void tiny_random_create(uint8_t *value, uint8_t length)
{
    (void)tiny_random_bytes(value, (size_t)length);
}
