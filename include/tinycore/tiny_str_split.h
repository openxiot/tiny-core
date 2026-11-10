/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_STR_SPLIT_H
#define TINYCORE_TINY_STR_SPLIT_H

#include <tinycore/tiny_base.h>

TINYCORE_BEGIN_DECLS

#define TINYCORE_STR_SPLIT_GROUP_SIZE 128U

/*
 * Split str on every occurrence of separator into fixed size groups.
 *
 * Empty fields are kept: "a--b" yields "a", "", "b".  Groups longer than
 * TINYCORE_STR_SPLIT_GROUP_SIZE - 1 bytes are truncated.  Returns the number
 * of groups written (at most group_count); 0 when an argument is invalid or
 * separator is empty.
 *
 * usage:
 *     char group[10][TINYCORE_STR_SPLIT_GROUP_SIZE];
 *     uint32_t count = tiny_str_split("urn:a:b", ":", group, 10);
 */
TINYCORE_API uint32_t tiny_str_split(const char *str,
                                     const char *separator,
                                     char group[][TINYCORE_STR_SPLIT_GROUP_SIZE],
                                     uint32_t group_count);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_STR_SPLIT_H */
