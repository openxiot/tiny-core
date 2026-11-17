/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/cycle_buffer.h>
#include <tinycore/tiny_alloc.h>

TinyRet CycleBuffer_Construct(CycleBuffer *buffer, size_t size)
{
    if (buffer == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    buffer->bytes = NULL;
    buffer->size = 0;
    buffer->length = 0;
    buffer->read_index = 0;
    buffer->write_index = 0;

    if (size == 0)
    {
        return TINY_RET_E_PARAM_INVALID;
    }

    buffer->bytes = (uint8_t *)tiny_calloc(size, sizeof(uint8_t));
    if (buffer->bytes == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    buffer->size = size;
    return TINY_RET_OK;
}

void CycleBuffer_Dispose(CycleBuffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    tiny_free(buffer->bytes);
    buffer->bytes = NULL;
    buffer->size = 0;
    buffer->length = 0;
    buffer->read_index = 0;
    buffer->write_index = 0;
}

CycleBuffer *CycleBuffer_New(size_t size)
{
    CycleBuffer *buffer = (CycleBuffer *)tiny_malloc(sizeof(CycleBuffer));

    if (buffer == NULL)
    {
        return NULL;
    }

    if (CycleBuffer_Construct(buffer, size) != TINY_RET_OK)
    {
        tiny_free(buffer);
        return NULL;
    }

    return buffer;
}

void CycleBuffer_Delete(CycleBuffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    CycleBuffer_Dispose(buffer);
    tiny_free(buffer);
}

void CycleBuffer_Reset(CycleBuffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    buffer->length = 0;
    buffer->read_index = 0;
    buffer->write_index = 0;
}

size_t CycleBuffer_GetSize(const CycleBuffer *buffer)
{
    return (buffer != NULL) ? buffer->size : 0U;
}

size_t CycleBuffer_GetLength(const CycleBuffer *buffer)
{
    return (buffer != NULL) ? buffer->length : 0U;
}

size_t CycleBuffer_GetFreeSize(const CycleBuffer *buffer)
{
    if (buffer == NULL || buffer->length > buffer->size)
    {
        return 0U;
    }

    return buffer->size - buffer->length;
}

bool CycleBuffer_IsEmpty(const CycleBuffer *buffer)
{
    return (buffer != NULL && buffer->length == 0);
}

bool CycleBuffer_IsFull(const CycleBuffer *buffer)
{
    return CycleBuffer_GetFreeSize(buffer) == 0U;
}

size_t CycleBuffer_Write(CycleBuffer *buffer, const void *data, size_t length)
{
    const uint8_t *source = (const uint8_t *)data;
    size_t free_size = 0;
    size_t count = 0;
    size_t first = 0;
    size_t second = 0;

    if (buffer == NULL || data == NULL || length == 0)
    {
        return 0U;
    }

    free_size = CycleBuffer_GetFreeSize(buffer);
    count = MIN(length, free_size);

    first = MIN(count, buffer->size - buffer->write_index);
    second = count - first;

    if (first > 0)
    {
        memcpy(buffer->bytes + buffer->write_index, source, first);
    }

    if (second > 0)
    {
        memcpy(buffer->bytes, source + first, second);
    }

    buffer->write_index = (buffer->write_index + count) % buffer->size;
    buffer->length += count;

    return count;
}

size_t CycleBuffer_Pick(const CycleBuffer *buffer, void *data, size_t max_length)
{
    return CycleBuffer_PickOffset(buffer, 0U, data, max_length);
}

size_t CycleBuffer_PickOffset(const CycleBuffer *buffer,
                              size_t offset,
                              void *data,
                              size_t max_length)
{
    uint8_t *destination = (uint8_t *)data;
    size_t readable = 0;
    size_t count = 0;
    size_t start = 0;
    size_t first = 0;
    size_t second = 0;

    if (buffer == NULL || data == NULL || max_length == 0)
    {
        return 0U;
    }

    readable = buffer->length;
    if (offset >= readable)
    {
        return 0U;
    }

    count = MIN(max_length, readable - offset);
    start = (buffer->read_index + offset) % buffer->size;

    first = MIN(count, buffer->size - start);
    second = count - first;

    memcpy(destination, buffer->bytes + start, first);
    if (second > 0)
    {
        memcpy(destination + first, buffer->bytes, second);
    }

    return count;
}

size_t CycleBuffer_Read(CycleBuffer *buffer, void *data, size_t max_length)
{
    size_t count = 0;

    if (buffer == NULL)
    {
        return 0U;
    }

    count = CycleBuffer_Pick(buffer, data, max_length);
    CycleBuffer_Skip(buffer, count);

    return count;
}

size_t CycleBuffer_Skip(CycleBuffer *buffer, size_t length)
{
    size_t count = 0;

    if (buffer == NULL || length == 0 || buffer->length == 0)
    {
        return 0U;
    }

    count = MIN(length, buffer->length);
    buffer->read_index = (buffer->read_index + count) % buffer->size;
    buffer->length -= count;

    if (buffer->length == 0)
    {
        buffer->read_index = 0;
        buffer->write_index = 0;
    }

    return count;
}
