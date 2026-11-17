/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/byte_buffer.h>
#include <tinycore/tiny_alloc.h>

TinyRet ByteBuffer_Construct(ByteBuffer *buffer, size_t size)
{
    if (buffer == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    buffer->bytes = NULL;
    buffer->size = 0;
    buffer->offset = 0;
    buffer->length = 0;

    if (size > 0)
    {
        buffer->bytes = (uint8_t *)tiny_calloc(size, sizeof(uint8_t));
        if (buffer->bytes == NULL)
        {
            return TINY_RET_E_NO_MEMORY;
        }

        buffer->size = size;
    }

    return TINY_RET_OK;
}

void ByteBuffer_Dispose(ByteBuffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    tiny_free(buffer->bytes);
    buffer->bytes = NULL;
    buffer->size = 0;
    buffer->offset = 0;
    buffer->length = 0;
}

ByteBuffer *ByteBuffer_New(size_t size)
{
    ByteBuffer *buffer = (ByteBuffer *)tiny_malloc(sizeof(ByteBuffer));

    if (buffer == NULL)
    {
        return NULL;
    }

    if (ByteBuffer_Construct(buffer, size) != TINY_RET_OK)
    {
        tiny_free(buffer);
        return NULL;
    }

    return buffer;
}

void ByteBuffer_Delete(ByteBuffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    ByteBuffer_Dispose(buffer);
    tiny_free(buffer);
}

void ByteBuffer_Clear(ByteBuffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    buffer->offset = 0;
    buffer->length = 0;
}

void ByteBuffer_Compact(ByteBuffer *buffer)
{
    size_t readable = 0;

    if (buffer == NULL)
    {
        return;
    }

    readable = ByteBuffer_GetReadableSize(buffer);

    if (readable > 0 && buffer->offset > 0)
    {
        memmove(buffer->bytes, buffer->bytes + buffer->offset, readable);
    }

    buffer->offset = 0;
    buffer->length = readable;
}

uint8_t *ByteBuffer_GetBytes(ByteBuffer *buffer)
{
    return (buffer != NULL) ? buffer->bytes : NULL;
}

const uint8_t *ByteBuffer_GetBytesConst(const ByteBuffer *buffer)
{
    return (buffer != NULL) ? buffer->bytes : NULL;
}

size_t ByteBuffer_GetSize(const ByteBuffer *buffer)
{
    return (buffer != NULL) ? buffer->size : 0U;
}

size_t ByteBuffer_GetLength(const ByteBuffer *buffer)
{
    return (buffer != NULL) ? buffer->length : 0U;
}

size_t ByteBuffer_GetOffset(const ByteBuffer *buffer)
{
    return (buffer != NULL) ? buffer->offset : 0U;
}

size_t ByteBuffer_GetReadableSize(const ByteBuffer *buffer)
{
    if (buffer == NULL || buffer->length < buffer->offset)
    {
        return 0U;
    }

    return buffer->length - buffer->offset;
}

size_t ByteBuffer_GetWritableSize(const ByteBuffer *buffer)
{
    if (buffer == NULL || buffer->length > buffer->size)
    {
        return 0U;
    }

    return buffer->size - buffer->length;
}

bool ByteBuffer_IsEmpty(const ByteBuffer *buffer)
{
    return ByteBuffer_GetReadableSize(buffer) == 0;
}

bool ByteBuffer_IsFull(const ByteBuffer *buffer)
{
    return ByteBuffer_GetWritableSize(buffer) == 0;
}

bool ByteBuffer_Put(ByteBuffer *buffer, uint8_t value)
{
    return ByteBuffer_Add(buffer, &value, 1U) == 1U;
}

size_t ByteBuffer_Add(ByteBuffer *buffer, const void *data, size_t length)
{
    size_t writable = 0;
    size_t count = 0;

    if (buffer == NULL || data == NULL || length == 0)
    {
        return 0U;
    }

    writable = ByteBuffer_GetWritableSize(buffer);
    count = MIN(length, writable);

    if (count > 0)
    {
        memcpy(buffer->bytes + buffer->length, data, count);
        buffer->length += count;
    }

    return count;
}

bool ByteBuffer_Get(ByteBuffer *buffer, uint8_t *value)
{
    if (buffer == NULL || value == NULL || ByteBuffer_GetReadableSize(buffer) == 0)
    {
        return false;
    }

    *value = buffer->bytes[buffer->offset];
    ++buffer->offset;

    return true;
}

size_t ByteBuffer_Read(ByteBuffer *buffer, void *data, size_t max_length)
{
    size_t count = 0;

    if (buffer == NULL)
    {
        return 0U;
    }

    count = ByteBuffer_Pick(buffer, data, max_length);
    buffer->offset += count; /* safe: Pick returned at most readable */

    return count;
}

size_t ByteBuffer_Pick(const ByteBuffer *buffer, void *data, size_t max_length)
{
    return ByteBuffer_PickOffset(buffer, 0U, data, max_length);
}

size_t ByteBuffer_PickOffset(const ByteBuffer *buffer,
                             size_t offset,
                             void *data,
                             size_t max_length)
{
    size_t readable = 0;
    size_t count = 0;

    if (buffer == NULL || data == NULL || max_length == 0)
    {
        return 0U;
    }

    readable = ByteBuffer_GetReadableSize(buffer);
    if (offset >= readable)
    {
        return 0U;
    }

    count = MIN(max_length, readable - offset);
    memcpy(data, buffer->bytes + buffer->offset + offset, count);

    return count;
}
