/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <tinycore/buffer.h>
#include <tinycore/byte_buffer.h>
#include <tinycore/cycle_buffer.h>

static void test_growable_buffer(void)
{
    Buffer buffer;

    TINY_TEST_CHECK(Buffer_Construct(&buffer, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_IsEmpty(&buffer));

    TINY_TEST_CHECK(Buffer_AppendString(&buffer, "hello") == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == 5);
    TINY_TEST_CHECK(memcmp(Buffer_GetBytesConst(&buffer), "hello", 5) == 0);
    TINY_TEST_CHECK(Buffer_GetBytes(&buffer)[5] == '\0');

    TINY_TEST_CHECK(Buffer_AppendByte(&buffer, '!') == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == 6);
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[5] == '!');
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[6] == '\0');

    TINY_TEST_CHECK(Buffer_Reserve(&buffer, 1024) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetCapacity(&buffer) >= 1024);

    TINY_TEST_CHECK(Buffer_SetLength(&buffer, 10) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == 10);
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[6] == '\0');
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[9] == '\0');

    Buffer_Clear(&buffer);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == 0);
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[0] == '\0');

    Buffer_Release(&buffer);
    TINY_TEST_CHECK(Buffer_GetCapacity(&buffer) == 0);

    Buffer_Dispose(&buffer);
}

static void test_byte_buffer(void)
{
    ByteBuffer buffer;
    uint8_t all[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t picked[8];
    uint8_t value = 0;

    TINY_TEST_CHECK(ByteBuffer_Construct(&buffer, 8) == TINY_RET_OK);
    TINY_TEST_CHECK(ByteBuffer_IsEmpty(&buffer));
    TINY_TEST_CHECK(ByteBuffer_GetSize(&buffer) == 8);
    TINY_TEST_CHECK(ByteBuffer_GetWritableSize(&buffer) == 8);

    TINY_TEST_CHECK(ByteBuffer_Add(&buffer, all, 4) == 4);
    TINY_TEST_CHECK(ByteBuffer_GetReadableSize(&buffer) == 4);
    TINY_TEST_CHECK(ByteBuffer_GetWritableSize(&buffer) == 4);

    TINY_TEST_CHECK(ByteBuffer_Get(&buffer, &value));
    TINY_TEST_CHECK(value == 1);

    TINY_TEST_CHECK(ByteBuffer_Pick(&buffer, picked, sizeof(picked)) == 3);
    TINY_TEST_CHECK(picked[0] == 2 && picked[2] == 4);

    TINY_TEST_CHECK(ByteBuffer_PickOffset(&buffer, 1, picked, sizeof(picked)) == 2);
    TINY_TEST_CHECK(picked[0] == 3 && picked[1] == 4);

    TINY_TEST_CHECK(ByteBuffer_Read(&buffer, picked, sizeof(picked)) == 3);
    TINY_TEST_CHECK(ByteBuffer_IsEmpty(&buffer));

    TINY_TEST_CHECK(ByteBuffer_Put(&buffer, 9));
    TINY_TEST_CHECK(ByteBuffer_GetWritableSize(&buffer) == 3);

    ByteBuffer_Compact(&buffer);
    TINY_TEST_CHECK(ByteBuffer_GetOffset(&buffer) == 0);
    TINY_TEST_CHECK(ByteBuffer_GetLength(&buffer) == 1);

    TINY_TEST_CHECK(ByteBuffer_Add(&buffer, all, 8) == 7);
    TINY_TEST_CHECK(ByteBuffer_IsFull(&buffer));

    ByteBuffer_Clear(&buffer);
    TINY_TEST_CHECK(ByteBuffer_IsEmpty(&buffer));
    TINY_TEST_CHECK(ByteBuffer_GetLength(&buffer) == 0);

    ByteBuffer_Dispose(&buffer);
}

static void test_cycle_buffer(void)
{
    CycleBuffer buffer;
    uint8_t data[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t out[8];

    TINY_TEST_CHECK(CycleBuffer_Construct(&buffer, 4) == TINY_RET_OK);
    TINY_TEST_CHECK(CycleBuffer_IsEmpty(&buffer));

    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, data, 3) == 3);
    TINY_TEST_CHECK(CycleBuffer_GetLength(&buffer) == 3);
    TINY_TEST_CHECK(CycleBuffer_GetFreeSize(&buffer) == 1);

    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, data + 3, 3) == 1);
    TINY_TEST_CHECK(CycleBuffer_IsFull(&buffer));

    TINY_TEST_CHECK(CycleBuffer_Pick(&buffer, out, sizeof(out)) == 4);
    TINY_TEST_CHECK(out[0] == 1 && out[1] == 2 && out[2] == 3 && out[3] == 4);

    TINY_TEST_CHECK(CycleBuffer_Read(&buffer, out, 2) == 2);
    TINY_TEST_CHECK(out[0] == 1 && out[1] == 2);

    /* Wrap around: two bytes free at the start of the ring. */
    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, data + 5, 2) == 2);
    TINY_TEST_CHECK(CycleBuffer_GetLength(&buffer) == 4);

    TINY_TEST_CHECK(CycleBuffer_Read(&buffer, out, sizeof(out)) == 4);
    TINY_TEST_CHECK(out[0] == 3 && out[1] == 4 && out[2] == 6 && out[3] == 7);
    TINY_TEST_CHECK(CycleBuffer_IsEmpty(&buffer));

    TINY_TEST_CHECK(CycleBuffer_Skip(&buffer, 0) == 0);
    TINY_TEST_CHECK(CycleBuffer_PickOffset(&buffer, 0, out, sizeof(out)) == 0);

    CycleBuffer_Reset(&buffer);
    CycleBuffer_Dispose(&buffer);
}

static void test_buffer_edges(void)
{
    Buffer buffer;
    Buffer *heap = Buffer_New(64);
    size_t capacity = 0;

    TINY_TEST_CHECK(Buffer_Construct(NULL, 0) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(heap != NULL);
    if (heap != NULL)
    {
        TINY_TEST_CHECK(Buffer_GetCapacity(heap) >= 64);
        TINY_TEST_CHECK(Buffer_AppendString(heap, "heap") == TINY_RET_OK);
        TINY_TEST_CHECK(Buffer_GetLength(heap) == 4);
        Buffer_Delete(heap);
    }

    TINY_TEST_CHECK(Buffer_Construct(&buffer, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_Reserve(NULL, 4) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Buffer_SetLength(NULL, 4) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Buffer_Append(NULL, "x", 1) == TINY_RET_E_NULL_PTR);

    TINY_TEST_CHECK(Buffer_Append(&buffer, NULL, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_Append(&buffer, NULL, 3) == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(Buffer_AppendString(&buffer, NULL) == TINY_RET_E_NULL_PTR);

    /* Growing zero fills the new bytes and keeps the terminator. */
    TINY_TEST_CHECK(Buffer_SetLength(&buffer, 10) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == 10);
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[0] == 0);
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[9] == 0);
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[10] == '\0');

    /* Reserving less than the current capacity changes nothing. */
    capacity = Buffer_GetCapacity(&buffer);
    TINY_TEST_CHECK(Buffer_Reserve(&buffer, 4) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetCapacity(&buffer) == capacity);
    TINY_TEST_CHECK(Buffer_Reserve(&buffer, capacity) == TINY_RET_OK);

    /* Shrinking keeps the terminator at the new end. */
    TINY_TEST_CHECK(Buffer_SetLength(&buffer, 3) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == 3);
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer)[3] == '\0');

    Buffer_Release(&buffer);
    TINY_TEST_CHECK(Buffer_GetCapacity(&buffer) == 0);
    TINY_TEST_CHECK(Buffer_GetBytesConst(&buffer) == NULL);
    TINY_TEST_CHECK(Buffer_AppendString(&buffer, "again") == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == 5);

    Buffer_Clear(NULL);
    Buffer_Release(NULL);
    Buffer_Dispose(NULL);
    Buffer_Delete(NULL);
    TINY_TEST_CHECK(Buffer_GetBytes(NULL) == NULL);
    TINY_TEST_CHECK(Buffer_GetBytesConst(NULL) == NULL);
    TINY_TEST_CHECK(Buffer_GetLength(NULL) == 0);
    TINY_TEST_CHECK(Buffer_GetCapacity(NULL) == 0);
    TINY_TEST_CHECK(Buffer_IsEmpty(NULL));

    Buffer_Dispose(&buffer);
}

static void test_byte_buffer_edges(void)
{
    ByteBuffer buffer;
    ByteBuffer empty;
    ByteBuffer *heap = NULL;
    ByteBuffer *zero_heap = NULL;
    uint8_t data[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t out[8];
    uint8_t value = 0;

    TINY_TEST_CHECK(ByteBuffer_Construct(NULL, 4) == TINY_RET_E_NULL_PTR);

    heap = ByteBuffer_New(4);
    TINY_TEST_CHECK(heap != NULL);
    if (heap != NULL)
    {
        TINY_TEST_CHECK(ByteBuffer_Add(heap, data, 2) == 2);
        TINY_TEST_CHECK(ByteBuffer_Get(heap, &value));
        TINY_TEST_CHECK(value == 1);
        ByteBuffer_Delete(heap);
    }

    zero_heap = ByteBuffer_New(0);
    TINY_TEST_CHECK(zero_heap != NULL);
    ByteBuffer_Delete(zero_heap);

    TINY_TEST_CHECK(ByteBuffer_Construct(&empty, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(ByteBuffer_GetSize(&empty) == 0);
    TINY_TEST_CHECK(ByteBuffer_GetWritableSize(&empty) == 0);
    TINY_TEST_CHECK(ByteBuffer_IsEmpty(&empty));
    TINY_TEST_CHECK(ByteBuffer_IsFull(&empty));
    TINY_TEST_CHECK(!ByteBuffer_Put(&empty, 1));
    TINY_TEST_CHECK(ByteBuffer_Add(&empty, data, 1) == 0);
    TINY_TEST_CHECK(!ByteBuffer_Get(&empty, &value));
    TINY_TEST_CHECK(ByteBuffer_Read(&empty, out, sizeof(out)) == 0);
    TINY_TEST_CHECK(ByteBuffer_Pick(&empty, out, sizeof(out)) == 0);
    TINY_TEST_CHECK(ByteBuffer_PickOffset(&empty, 0, out, sizeof(out)) == 0);
    ByteBuffer_Compact(&empty);
    ByteBuffer_Clear(&empty);
    ByteBuffer_Dispose(&empty);

    TINY_TEST_CHECK(ByteBuffer_Construct(&buffer, 4) == TINY_RET_OK);
    TINY_TEST_CHECK(ByteBuffer_Add(NULL, data, 2) == 0);
    TINY_TEST_CHECK(ByteBuffer_Add(&buffer, NULL, 2) == 0);
    TINY_TEST_CHECK(ByteBuffer_Add(&buffer, data, 0) == 0);
    TINY_TEST_CHECK(!ByteBuffer_Get(NULL, &value));
    TINY_TEST_CHECK(ByteBuffer_Read(NULL, out, sizeof(out)) == 0);
    TINY_TEST_CHECK(ByteBuffer_Pick(NULL, out, sizeof(out)) == 0);

    TINY_TEST_CHECK(ByteBuffer_Add(&buffer, data, 2) == 2);
    TINY_TEST_CHECK(ByteBuffer_Read(&buffer, out, sizeof(out)) == 2);
    TINY_TEST_CHECK(out[0] == 1 && out[1] == 2);
    TINY_TEST_CHECK(!ByteBuffer_Get(&buffer, &value));

    /* Read space is not reclaimed until Compact(): only two bytes remain writable. */
    TINY_TEST_CHECK(ByteBuffer_Add(&buffer, data, 8) == 2);
    TINY_TEST_CHECK(ByteBuffer_GetLength(&buffer) == 4);
    TINY_TEST_CHECK(ByteBuffer_IsFull(&buffer));
    TINY_TEST_CHECK(ByteBuffer_PickOffset(&buffer, 100, out, sizeof(out)) == 0);
    TINY_TEST_CHECK(ByteBuffer_PickOffset(&buffer, 0, NULL, sizeof(out)) == 0);

    ByteBuffer_Compact(NULL);
    ByteBuffer_Clear(NULL);
    ByteBuffer_Dispose(NULL);
    ByteBuffer_Delete(NULL);
    TINY_TEST_CHECK(ByteBuffer_GetSize(NULL) == 0);
    TINY_TEST_CHECK(ByteBuffer_GetLength(NULL) == 0);
    TINY_TEST_CHECK(ByteBuffer_GetOffset(NULL) == 0);
    TINY_TEST_CHECK(ByteBuffer_GetReadableSize(NULL) == 0);
    TINY_TEST_CHECK(ByteBuffer_GetWritableSize(NULL) == 0);
    TINY_TEST_CHECK(ByteBuffer_GetBytes(NULL) == NULL);
    TINY_TEST_CHECK(ByteBuffer_GetBytesConst(NULL) == NULL);

    ByteBuffer_Dispose(&buffer);
}

static void test_cycle_buffer_edges(void)
{
    CycleBuffer buffer;
    CycleBuffer *heap = NULL;
    uint8_t data[4] = { 1, 2, 3, 4 };
    uint8_t out[8];

    TINY_TEST_CHECK(CycleBuffer_Construct(NULL, 4) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(CycleBuffer_Construct(&buffer, 0) == TINY_RET_E_PARAM_INVALID);

    heap = CycleBuffer_New(4);
    TINY_TEST_CHECK(heap != NULL);
    if (heap != NULL)
    {
        TINY_TEST_CHECK(CycleBuffer_Write(heap, data, 4) == 4);
        TINY_TEST_CHECK(CycleBuffer_Read(heap, out, sizeof(out)) == 4);
        CycleBuffer_Delete(heap);
    }
    TINY_TEST_CHECK(CycleBuffer_New(0) == NULL);

    TINY_TEST_CHECK(CycleBuffer_Construct(&buffer, 4) == TINY_RET_OK);
    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, NULL, 2) == 0);
    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, data, 0) == 0);
    TINY_TEST_CHECK(CycleBuffer_Read(&buffer, out, sizeof(out)) == 0);
    TINY_TEST_CHECK(CycleBuffer_Pick(&buffer, NULL, sizeof(out)) == 0);
    TINY_TEST_CHECK(CycleBuffer_Pick(&buffer, out, 0) == 0);
    TINY_TEST_CHECK(CycleBuffer_PickOffset(&buffer, 0, out, sizeof(out)) == 0);
    TINY_TEST_CHECK(CycleBuffer_Skip(&buffer, 0) == 0);

    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, data, 3) == 3);
    TINY_TEST_CHECK(CycleBuffer_PickOffset(&buffer, 2, out, sizeof(out)) == 1);
    TINY_TEST_CHECK(out[0] == 3);
    TINY_TEST_CHECK(CycleBuffer_PickOffset(&buffer, 3, out, sizeof(out)) == 0);

    /* Reading past the end consumes only what is available and rewinds. */
    TINY_TEST_CHECK(CycleBuffer_Read(&buffer, out, sizeof(out)) == 3);
    TINY_TEST_CHECK(out[0] == 1 && out[2] == 3);
    TINY_TEST_CHECK(CycleBuffer_GetLength(&buffer) == 0);
    TINY_TEST_CHECK(buffer.read_index == 0 && buffer.write_index == 0);

    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, data, 2) == 2);
    TINY_TEST_CHECK(CycleBuffer_Skip(&buffer, 10) == 2);
    TINY_TEST_CHECK(CycleBuffer_IsEmpty(&buffer));
    TINY_TEST_CHECK(buffer.read_index == 0 && buffer.write_index == 0);

    TINY_TEST_CHECK(!CycleBuffer_IsEmpty(NULL));
    TINY_TEST_CHECK(CycleBuffer_GetSize(NULL) == 0);
    TINY_TEST_CHECK(CycleBuffer_GetLength(NULL) == 0);
    TINY_TEST_CHECK(CycleBuffer_GetFreeSize(NULL) == 0);
    CycleBuffer_Reset(NULL);
    CycleBuffer_Dispose(NULL);
    CycleBuffer_Delete(NULL);

    CycleBuffer_Dispose(&buffer);
}

int main(void)
{
    TINY_TEST_BEGIN("buffer");
    test_growable_buffer();
    test_buffer_edges();
    test_byte_buffer();
    test_byte_buffer_edges();
    test_cycle_buffer();
    test_cycle_buffer_edges();
    return TINY_TEST_FINISH();
}
