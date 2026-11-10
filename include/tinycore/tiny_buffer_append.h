/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_BUFFER_APPEND_H
#define TINYCORE_TINY_BUFFER_APPEND_H

#include <tinycore/tiny_base.h>

TINYCORE_BEGIN_DECLS

/*
 * Bounded append into a caller owned fixed buffer.
 *
 * buffer   storage to write into (may be NULL)
 * capacity total bytes available in buffer
 * offset   write position; must be <= capacity
 *
 * Each function returns the number of bytes written (0 on failure, when the
 * inputs are invalid or the value does not fit).  The functions only return
 * the number written, it is the caller's job to advance its own offset.
 */
TINYCORE_API size_t tiny_buffer_append_byte(uint8_t *buffer,
                                            size_t capacity,
                                            size_t offset,
                                            uint8_t value);

TINYCORE_API size_t tiny_buffer_append_bytes(uint8_t *buffer,
                                             size_t capacity,
                                             size_t offset,
                                             const void *data,
                                             size_t length);

/* Appends the string without its NUL terminator. */
TINYCORE_API size_t tiny_buffer_append_string(char *buffer,
                                              size_t capacity,
                                              size_t offset,
                                              const char *string);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_BUFFER_APPEND_H */
