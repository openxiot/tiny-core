/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_CYCLE_BUFFER_H
#define TINYCORE_CYCLE_BUFFER_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/* Ring buffer.  New bytes are written at write_index and consumed from
 * read_index; length counts the unread bytes.  Writes never overwrite unread
 * data: when the buffer is full the extra bytes are dropped and the caller
 * is told how many were accepted. */
typedef struct CycleBuffer
{
    uint8_t *bytes;
    size_t size;
    size_t length;
    size_t read_index;
    size_t write_index;
} CycleBuffer;

TINYCORE_API TINYCORE_RET CycleBuffer_Construct(CycleBuffer *buffer, size_t size);
TINYCORE_API void CycleBuffer_Dispose(CycleBuffer *buffer);
TINYCORE_API CycleBuffer *CycleBuffer_New(size_t size);
TINYCORE_API void CycleBuffer_Delete(CycleBuffer *buffer);

/* Drop every unread byte. */
TINYCORE_API void CycleBuffer_Reset(CycleBuffer *buffer);

TINYCORE_API size_t CycleBuffer_GetSize(const CycleBuffer *buffer);
TINYCORE_API size_t CycleBuffer_GetLength(const CycleBuffer *buffer);
TINYCORE_API size_t CycleBuffer_GetFreeSize(const CycleBuffer *buffer);
TINYCORE_API bool CycleBuffer_IsEmpty(const CycleBuffer *buffer);
TINYCORE_API bool CycleBuffer_IsFull(const CycleBuffer *buffer);

/* Append up to length bytes; returns the number accepted. */
TINYCORE_API size_t CycleBuffer_Write(CycleBuffer *buffer, const void *data, size_t length);

/* Copy up to max_length unread bytes without consuming them. */
TINYCORE_API size_t CycleBuffer_Pick(const CycleBuffer *buffer, void *data, size_t max_length);

/* Copy up to max_length bytes starting offset bytes after the read
 * position, without consuming them. */
TINYCORE_API size_t CycleBuffer_PickOffset(const CycleBuffer *buffer,
                                           size_t offset,
                                           void *data,
                                           size_t max_length);

/* Copy and consume up to max_length bytes; returns the number consumed. */
TINYCORE_API size_t CycleBuffer_Read(CycleBuffer *buffer, void *data, size_t max_length);

/* Consume up to length bytes without copying; returns the number consumed. */
TINYCORE_API size_t CycleBuffer_Skip(CycleBuffer *buffer, size_t length);

TINYCORE_END_DECLS

#endif /* TINYCORE_CYCLE_BUFFER_H */
