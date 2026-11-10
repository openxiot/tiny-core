/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_log.h>
#include <tinycore/tiny_snprintf.h>

#include <stdio.h>

#define TINYCORE_BINARY_BYTES_PER_LINE 16

void tiny_print_binary(const void *data, size_t size)
{
    const uint8_t *bytes = (const uint8_t *)data;
    char line[TINYCORE_BINARY_BYTES_PER_LINE * 4 + 72];
    size_t offset = 0;

    if (data == NULL)
    {
        return;
    }

    for (offset = 0; offset < size; offset += TINYCORE_BINARY_BYTES_PER_LINE)
    {
        size_t i = 0;
        int position = 0;
        size_t remaining = size - offset;

        position += tiny_snprintf(line + position,
                                  sizeof(line) - (size_t)position,
                                  "%08zx  ",
                                  offset);

        for (i = 0; i < TINYCORE_BINARY_BYTES_PER_LINE; ++i)
        {
            if (i < remaining)
            {
                position += tiny_snprintf(line + position,
                                          sizeof(line) - (size_t)position,
                                          "%02x ",
                                          bytes[offset + i]);
            }
            else
            {
                position += tiny_snprintf(line + position,
                                          sizeof(line) - (size_t)position,
                                          "   ");
            }
        }

        position += tiny_snprintf(line + position,
                                  sizeof(line) - (size_t)position,
                                  " |");

        for (i = 0; i < TINYCORE_BINARY_BYTES_PER_LINE && i < remaining; ++i)
        {
            uint8_t value = bytes[offset + i];

            position += tiny_snprintf(line + position,
                                      sizeof(line) - (size_t)position,
                                      "%c",
                                      (value >= 0x20U && value < 0x7FU) ? (char)value : '.');
        }

        tiny_snprintf(line + position, sizeof(line) - (size_t)position, "|");

        TINY_LOG_INFO("%s", line);
    }
}
