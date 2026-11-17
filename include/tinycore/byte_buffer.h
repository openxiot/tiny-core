/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_BYTE_BUFFER_H
#define TINYCORE_BYTE_BUFFER_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/* Fixed capacity linear byte buffer.
 *
 *   [ consumed | unread | free ]
 *      offset    length   size
 *
 * Bytes are appended at length and consumed at offset.  Compact() moves the
 * unread region back to the start so the free region becomes contiguous. */
typedef struct
{
    uint8_t *bytes;
    size_t size;   /* capacity */
    size_t offset; /* read position */
    size_t length; /* write position */
} ByteBuffer;

TINYCORE_API TinyRet ByteBuffer_Construct(ByteBuffer *buffer, size_t size);
TINYCORE_API void ByteBuffer_Dispose(ByteBuffer *buffer);
TINYCORE_API ByteBuffer *ByteBuffer_New(size_t size);
TINYCORE_API void ByteBuffer_Delete(ByteBuffer *buffer);

/* Reset the read and write positions. */
TINYCORE_API void ByteBuffer_Clear(ByteBuffer *buffer);

/* Move the unread bytes back to the start of the storage. */
TINYCORE_API void ByteBuffer_Compact(ByteBuffer *buffer);

TINYCORE_API uint8_t *ByteBuffer_GetBytes(ByteBuffer *buffer);
TINYCORE_API const uint8_t *ByteBuffer_GetBytesConst(const ByteBuffer *buffer);
TINYCORE_API size_t ByteBuffer_GetSize(const ByteBuffer *buffer);
TINYCORE_API size_t ByteBuffer_GetLength(const ByteBuffer *buffer);
TINYCORE_API size_t ByteBuffer_GetOffset(const ByteBuffer *buffer);

/* Bytes available for reading. */
TINYCORE_API size_t ByteBuffer_GetReadableSize(const ByteBuffer *buffer);

/* Bytes available for writing before Compact() is needed. */
TINYCORE_API size_t ByteBuffer_GetWritableSize(const ByteBuffer *buffer);

TINYCORE_API bool ByteBuffer_IsEmpty(const ByteBuffer *buffer);
TINYCORE_API bool ByteBuffer_IsFull(const ByteBuffer *buffer);

/* Append one byte / a run of bytes.  Returns false when the buffer is full. */
TINYCORE_API bool ByteBuffer_Put(ByteBuffer *buffer, uint8_t value);
TINYCORE_API size_t ByteBuffer_Add(ByteBuffer *buffer, const void *data, size_t length);

/* Consume one byte / up to max_length bytes.  Returns the consumed count. */
TINYCORE_API bool ByteBuffer_Get(ByteBuffer *buffer, uint8_t *value);
TINYCORE_API size_t ByteBuffer_Read(ByteBuffer *buffer, void *data, size_t max_length);

/* Copy without consuming. */
TINYCORE_API size_t ByteBuffer_Pick(const ByteBuffer *buffer, void *data, size_t max_length);

/* Copy without consuming, starting offset bytes after the read position. */
TINYCORE_API size_t ByteBuffer_PickOffset(const ByteBuffer *buffer,
                                          size_t offset,
                                          void *data,
                                          size_t max_length);

TINYCORE_END_DECLS

#endif /* TINYCORE_BYTE_BUFFER_H */
