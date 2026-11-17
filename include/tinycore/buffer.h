/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_BUFFER_H
#define TINYCORE_BUFFER_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/* Growable byte buffer.  The internal storage is always followed by a NUL
 * byte, so a buffer holding text can be passed to C string functions. */
typedef struct
{
    uint8_t *bytes;
    size_t length;   /* valid bytes */
    size_t capacity; /* allocated bytes, excluding the trailing NUL */
} Buffer;

/* Construct an empty buffer with room for capacity bytes.  A zero capacity
 * is allowed; the buffer grows on demand. */
TINYCORE_API TinyRet Buffer_Construct(Buffer *buffer, size_t capacity);
TINYCORE_API void Buffer_Dispose(Buffer *buffer);
TINYCORE_API Buffer *Buffer_New(size_t capacity);
TINYCORE_API void Buffer_Delete(Buffer *buffer);

TINYCORE_API uint8_t *Buffer_GetBytes(Buffer *buffer);
TINYCORE_API const uint8_t *Buffer_GetBytesConst(const Buffer *buffer);
TINYCORE_API size_t Buffer_GetLength(const Buffer *buffer);
TINYCORE_API size_t Buffer_GetCapacity(const Buffer *buffer);
TINYCORE_API bool Buffer_IsEmpty(const Buffer *buffer);

/* Ensure room for at least capacity bytes. */
TINYCORE_API TinyRet Buffer_Reserve(Buffer *buffer, size_t capacity);

/* Set the number of valid bytes, growing the storage when needed.  Bytes
 * between the old and new length are zero filled. */
TINYCORE_API TinyRet Buffer_SetLength(Buffer *buffer, size_t length);

/* Append bytes, growing the storage when needed. */
TINYCORE_API TinyRet Buffer_Append(Buffer *buffer, const void *data, size_t length);
TINYCORE_API TinyRet Buffer_AppendByte(Buffer *buffer, uint8_t value);

/* Append a NUL terminated string (without its NUL). */
TINYCORE_API TinyRet Buffer_AppendString(Buffer *buffer, const char *string);

/* Set the valid length to zero.  The storage is kept. */
TINYCORE_API void Buffer_Clear(Buffer *buffer);

/* Release the storage; the buffer stays usable. */
TINYCORE_API void Buffer_Release(Buffer *buffer);

TINYCORE_END_DECLS

#endif /* TINYCORE_BUFFER_H */
