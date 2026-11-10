/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_buffer_append.h>

size_t tiny_buffer_append_byte(uint8_t *buffer,
                               size_t capacity,
                               size_t offset,
                               uint8_t value)
{
    if (buffer == NULL || offset >= capacity)
    {
        return 0;
    }

    buffer[offset] = value;
    return 1;
}

size_t tiny_buffer_append_bytes(uint8_t *buffer,
                                size_t capacity,
                                size_t offset,
                                const void *data,
                                size_t length)
{
    if (buffer == NULL || data == NULL || offset > capacity || length > capacity - offset)
    {
        return 0;
    }

    if (length > 0)
    {
        memcpy(buffer + offset, data, length);
    }

    return length;
}

size_t tiny_buffer_append_string(char *buffer,
                                 size_t capacity,
                                 size_t offset,
                                 const char *string)
{
    if (string == NULL)
    {
        return 0;
    }

    return tiny_buffer_append_bytes((uint8_t *)buffer,
                                    capacity,
                                    offset,
                                    string,
                                    strlen(string));
}
