/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/buffer.h>
#include <tinycore/tiny_alloc.h>

#define TINYCORE_BUFFER_MIN_CAPACITY 16U

static TINYCORE_RET buffer_grow(Buffer *buffer, size_t needed)
{
    size_t capacity = buffer->capacity;
    uint8_t *bytes = NULL;

    if (needed <= capacity)
    {
        return TINY_RET_OK;
    }

    if (capacity == 0)
    {
        capacity = TINYCORE_BUFFER_MIN_CAPACITY;
    }

    while (capacity < needed)
    {
        if (capacity > (SIZE_MAX / 2U))
        {
            capacity = needed;
            break;
        }

        capacity *= 2U;
    }

    /* +1: the storage is always NUL terminated. */
    bytes = (uint8_t *)tiny_realloc(buffer->bytes, capacity + 1U);
    if (bytes == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    buffer->bytes = bytes;
    buffer->capacity = capacity;
    buffer->bytes[buffer->length] = '\0';

    return TINY_RET_OK;
}

TINYCORE_RET Buffer_Construct(Buffer *buffer, size_t capacity)
{
    if (buffer == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    buffer->bytes = NULL;
    buffer->length = 0;
    buffer->capacity = 0;

    if (capacity > 0)
    {
        return buffer_grow(buffer, capacity);
    }

    return TINY_RET_OK;
}

void Buffer_Dispose(Buffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    tiny_free(buffer->bytes);
    buffer->bytes = NULL;
    buffer->length = 0;
    buffer->capacity = 0;
}

Buffer *Buffer_New(size_t capacity)
{
    Buffer *buffer = (Buffer *)tiny_malloc(sizeof(Buffer));

    if (buffer == NULL)
    {
        return NULL;
    }

    if (Buffer_Construct(buffer, capacity) != TINY_RET_OK)
    {
        tiny_free(buffer);
        return NULL;
    }

    return buffer;
}

void Buffer_Delete(Buffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    Buffer_Dispose(buffer);
    tiny_free(buffer);
}

uint8_t *Buffer_GetBytes(Buffer *buffer)
{
    return (buffer != NULL) ? buffer->bytes : NULL;
}

const uint8_t *Buffer_GetBytesConst(const Buffer *buffer)
{
    return (buffer != NULL) ? buffer->bytes : NULL;
}

size_t Buffer_GetLength(const Buffer *buffer)
{
    return (buffer != NULL) ? buffer->length : 0U;
}

size_t Buffer_GetCapacity(const Buffer *buffer)
{
    return (buffer != NULL) ? buffer->capacity : 0U;
}

bool Buffer_IsEmpty(const Buffer *buffer)
{
    return (buffer == NULL) || (buffer->length == 0);
}

TINYCORE_RET Buffer_Reserve(Buffer *buffer, size_t capacity)
{
    if (buffer == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    return buffer_grow(buffer, capacity);
}

TINYCORE_RET Buffer_SetLength(Buffer *buffer, size_t length)
{
    size_t old_length = 0;
    TINYCORE_RET ret = TINY_RET_OK;

    if (buffer == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    ret = buffer_grow(buffer, length);
    if (ret != TINY_RET_OK)
    {
        return ret;
    }

    old_length = buffer->length;
    buffer->length = length;

    if (length > old_length && buffer->bytes != NULL)
    {
        memset(buffer->bytes + old_length, 0, length - old_length);
    }

    if (buffer->bytes != NULL)
    {
        buffer->bytes[length] = '\0';
    }

    return TINY_RET_OK;
}

TINYCORE_RET Buffer_Append(Buffer *buffer, const void *data, size_t length)
{
    TINYCORE_RET ret = TINY_RET_OK;

    if (buffer == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (data == NULL && length > 0)
    {
        return TINY_RET_E_PARAM_INVALID;
    }

    if (length == 0)
    {
        return TINY_RET_OK;
    }

    ret = buffer_grow(buffer, buffer->length + length);
    if (ret != TINY_RET_OK)
    {
        return ret;
    }

    memcpy(buffer->bytes + buffer->length, data, length);
    buffer->length += length;
    buffer->bytes[buffer->length] = '\0';

    return TINY_RET_OK;
}

TINYCORE_RET Buffer_AppendByte(Buffer *buffer, uint8_t value)
{
    return Buffer_Append(buffer, &value, 1U);
}

TINYCORE_RET Buffer_AppendString(Buffer *buffer, const char *string)
{
    if (string == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    return Buffer_Append(buffer, string, strlen(string));
}

void Buffer_Clear(Buffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    buffer->length = 0;

    if (buffer->bytes != NULL)
    {
        buffer->bytes[0] = '\0';
    }
}

void Buffer_Release(Buffer *buffer)
{
    Buffer_Dispose(buffer);
}
