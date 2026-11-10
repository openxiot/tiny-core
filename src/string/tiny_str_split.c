/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_str_split.h>

uint32_t tiny_str_split(const char *str,
                        const char *separator,
                        char group[][TINYCORE_STR_SPLIT_GROUP_SIZE],
                        uint32_t group_count)
{
    size_t separator_length = 0;
    const char *cursor = NULL;
    uint32_t count = 0;

    if (str == NULL || separator == NULL || group == NULL || group_count == 0)
    {
        return 0;
    }

    separator_length = strlen(separator);
    if (separator_length == 0)
    {
        return 0;
    }

    cursor = str;

    while (count < group_count)
    {
        const char *next = strstr(cursor, separator);
        size_t length = (next != NULL) ? (size_t)(next - cursor) : strlen(cursor);

        if (length >= TINYCORE_STR_SPLIT_GROUP_SIZE)
        {
            length = TINYCORE_STR_SPLIT_GROUP_SIZE - 1U;
        }

        memcpy(group[count], cursor, length);
        group[count][length] = '\0';
        ++count;

        if (next == NULL)
        {
            break;
        }

        cursor = next + separator_length;
    }

    return count;
}
