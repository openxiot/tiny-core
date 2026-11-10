/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <string.h>

#include <tinycore/json.h>
#include <tinycore/tlv8.h>

static void test_item(void)
{
    Tlv8Item item;
    Tlv8Item *heap = NULL;

    TINY_TEST_CHECK(Tlv8Item_Construct(&item) == TINY_RET_OK);
    TINY_TEST_CHECK(item.type == 0 && item.length == 0 && item.data == NULL);

    TINY_TEST_CHECK(Tlv8Item_Set(&item, 1, "abc", 3) == TINY_RET_OK);
    TINY_TEST_CHECK(item.type == 1 && item.length == 3);
    TINY_TEST_CHECK(memcmp(item.data, "abc", 3) == 0);

    TINY_TEST_CHECK(Tlv8Item_Append(&item, "de", 2) == TINY_RET_OK);
    TINY_TEST_CHECK(item.length == 5);
    TINY_TEST_CHECK(memcmp(item.data, "abcde", 5) == 0);

    /* Appending nothing is a no-op, even without data. */
    TINY_TEST_CHECK(Tlv8Item_Append(&item, NULL, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(item.length == 5);
    TINY_TEST_CHECK(Tlv8Item_Append(&item, NULL, 1) == TINY_RET_E_NULL_PTR);

    TINY_TEST_CHECK(Tlv8Item_Set(&item, 2, "", 0) == TINY_RET_OK);
    TINY_TEST_CHECK(item.length == 0);

    Tlv8Item_Dispose(&item);
    TINY_TEST_CHECK(item.data == NULL && item.length == 0);

    heap = Tlv8Item_New();
    TINY_TEST_CHECK(heap != NULL);
    if (heap != NULL)
    {
        TINY_TEST_CHECK(Tlv8Item_Set(heap, 3, "x", 1) == TINY_RET_OK);
        Tlv8Item_Delete(heap);
    }
}

static void test_list_serialization(void)
{
    Tlv8List list;
    const uint8_t *bytes = NULL;

    TINY_TEST_CHECK(Tlv8List_Construct(&list) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetCount(&list) == 0);
    TINY_TEST_CHECK(Tlv8List_GetBytes(&list) == NULL);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 0);

    TINY_TEST_CHECK(Tlv8List_AddBytes(&list, 1, "abc", 3, false) == TINY_RET_OK);
    bytes = Tlv8List_GetBytes(&list);
    TINY_TEST_CHECK(bytes != NULL);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 5);
    if (bytes != NULL)
    {
        TINY_TEST_CHECK(bytes[0] == 1 && bytes[1] == 3);
        TINY_TEST_CHECK(memcmp(bytes + 2, "abc", 3) == 0);
    }

    TINY_TEST_CHECK(Tlv8List_AddSeparator(&list) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 7);
    bytes = Tlv8List_GetBytes(&list);
    TINY_TEST_CHECK(bytes != NULL && bytes[5] == 0xFF && bytes[6] == 0x00);

    TINY_TEST_CHECK(Tlv8List_AddBytes(&list, 0xFF, "x", 1, false) == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(Tlv8List_AddBytes(&list, 2, NULL, 1, false) == TINY_RET_E_NULL_PTR);

    /* Zero length values still get a two byte header. */
    TINY_TEST_CHECK(Tlv8List_AddBytes(&list, 2, NULL, 0, false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 9);

    Tlv8List_Clear(&list);
    TINY_TEST_CHECK(Tlv8List_GetCount(&list) == 0);
    TINY_TEST_CHECK(Tlv8List_GetBytes(&list) == NULL);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 0);

    Tlv8List_Dispose(&list);
}

static void test_fragmentation(void)
{
    Tlv8List list;
    uint8_t value[511];
    const uint8_t *bytes = NULL;
    size_t i = 0;

    for (i = 0; i < sizeof(value); ++i)
    {
        value[i] = (uint8_t)(i & 0xFFU);
    }

    TINY_TEST_CHECK(Tlv8List_Construct(&list) == TINY_RET_OK);

    TINY_TEST_CHECK(Tlv8List_AddBytes(&list, 1, value, 255, false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 257);

    TINY_TEST_CHECK(Tlv8List_AddBytes(&list, 2, value, 256, false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 257 + 260);

    TINY_TEST_CHECK(Tlv8List_AddBytes(&list, 3, value, 510, false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 257 + 260 + 514);

    TINY_TEST_CHECK(Tlv8List_AddBytes(&list, 4, value, 511, false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetSize(&list) == 257 + 260 + 514 + 517);

    bytes = Tlv8List_GetBytes(&list);
    TINY_TEST_CHECK(bytes != NULL);
    if (bytes != NULL)
    {
        /* type 2: first fragment 255 bytes, second fragment 1 byte */
        TINY_TEST_CHECK(bytes[257] == 2 && bytes[258] == 255);
        TINY_TEST_CHECK(bytes[257 + 2 + 255] == 2 && bytes[257 + 2 + 255 + 1] == 1);

        /* type 3: 255 + 255 bytes, two headers */
        {
            size_t base = 257 + 260;
            TINY_TEST_CHECK(bytes[base] == 3 && bytes[base + 1] == 255);
            TINY_TEST_CHECK(bytes[base + 257] == 3 && bytes[base + 257 + 1] == 255);
        }
    }

    Tlv8List_Dispose(&list);
}

static void test_parse(void)
{
    Tlv8List source;
    Tlv8List parsed;
    uint8_t data[512];
    uint8_t malformed[3] = { 0x01, 0x05, 0x00 };
    size_t i = 0;

    for (i = 0; i < sizeof(data); ++i)
    {
        data[i] = (uint8_t)(i & 0xFFU);
    }

    TINY_TEST_CHECK(Tlv8List_Construct(&source) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_AddBytes(&source, 1, "hello", 5, false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_AddByte(&source, 2, 0x42, false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_AddSeparator(&source) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_AddBytes(&source, 3, data, 510, false) == TINY_RET_OK);

    TINY_TEST_CHECK(Tlv8List_Construct(&parsed) == TINY_RET_OK);

    TINY_TEST_CHECK(Tlv8List_Parse(&parsed,
                                   Tlv8List_GetBytes(&source),
                                   Tlv8List_GetSize(&source),
                                   true) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetCount(&parsed) == 4);
    TINY_TEST_CHECK(Tlv8List_Get(&parsed, 1) != NULL);
    TINY_TEST_CHECK(Tlv8List_Get(&parsed, 1)->length == 5);
    TINY_TEST_CHECK(memcmp(Tlv8List_Get(&parsed, 1)->data, "hello", 5) == 0);
    TINY_TEST_CHECK(Tlv8List_Get(&parsed, 2) != NULL);
    TINY_TEST_CHECK(Tlv8List_Get(&parsed, 2)->length == 1);
    TINY_TEST_CHECK(Tlv8List_Get(&parsed, 3) != NULL);
    TINY_TEST_CHECK(Tlv8List_Get(&parsed, 3)->length == 510);
    TINY_TEST_CHECK(memcmp(Tlv8List_Get(&parsed, 3)->data, data, 255) == 0);
    TINY_TEST_CHECK(memcmp(Tlv8List_Get(&parsed, 3)->data + 255, data + 255, 255) == 0);

    Tlv8List_Clear(&parsed);
    TINY_TEST_CHECK(Tlv8List_Parse(&parsed,
                                   Tlv8List_GetBytes(&source),
                                   Tlv8List_GetSize(&source),
                                   false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetCount(&parsed) == 5);
    TINY_TEST_CHECK(Tlv8List_GetAt(&parsed, 4)->type == 3);
    TINY_TEST_CHECK(Tlv8List_GetAt(&parsed, 4)->length == 255);
    TINY_TEST_CHECK(Tlv8List_GetAt(&parsed, 5) == NULL);

    TINY_TEST_CHECK(Tlv8List_Parse(&parsed, malformed, sizeof(malformed), false) == TINY_RET_E_INVALID_DATA);
    TINY_TEST_CHECK(Tlv8List_Parse(&parsed, NULL, 1, false) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Tlv8List_Parse(NULL, data, sizeof(data), false) == TINY_RET_E_NULL_PTR);

    Tlv8List_Dispose(&parsed);
    Tlv8List_Dispose(&source);
}

static void test_factory_integers(void)
{
    static const int32_t values[] = { 0, 1, -1, 100, 200, 255, 256, 300, 32767, 65535,
                                      -100, -65535, 2147483647, -2147483648 };
    size_t i = 0;

    for (i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    {
        JsonValue *input = JsonValue_NewInteger(values[i]);
        Tlv8Item *item = Tlv8Item_NewFromJsonValue(input);
        JsonValue *output = NULL;

        TINY_TEST_CHECK(input != NULL);
        TINY_TEST_CHECK(item != NULL);
        TINY_TEST_CHECK(item != NULL && item->type == TLV8_VALUE_INTEGER);
        TINY_TEST_CHECK(item != NULL && item->length >= 1 && item->length <= 4);

        output = (item != NULL) ? JsonValue_NewFromTlv8(item) : NULL;
        TINY_TEST_CHECK(output != NULL);
        if (output != NULL)
        {
            TINY_TEST_CHECK(output->type == JSON_NUMBER);
            TINY_TEST_CHECK(JsonNumber_IsInteger(output->data.number));
            TINY_TEST_CHECK(JsonNumber_GetInteger(output->data.number) == values[i]);
        }

        JsonValue_Delete(output);
        Tlv8Item_Delete(item);
        JsonValue_Delete(input);
    }
}

static void test_factory_other_types(void)
{
    JsonValue *input = JsonValue_NewFloat(3.141592);
    Tlv8Item *item = Tlv8Item_NewFromJsonValue(input);
    JsonValue *output = NULL;

    TINY_TEST_CHECK(item != NULL && item->type == TLV8_VALUE_FLOAT && item->length == 4);
    output = (item != NULL) ? JsonValue_NewFromTlv8(item) : NULL;
    TINY_TEST_CHECK(output != NULL);
    if (output != NULL)
    {
        double value = JsonNumber_GetFloat(output->data.number);
        TINY_TEST_CHECK(value > 3.141590 && value < 3.141594);
    }
    JsonValue_Delete(output);
    Tlv8Item_Delete(item);
    JsonValue_Delete(input);

    input = JsonValue_NewBoolean(true);
    item = Tlv8Item_NewFromJsonValue(input);
    output = (item != NULL) ? JsonValue_NewFromTlv8(item) : NULL;
    TINY_TEST_CHECK(item != NULL && item->type == TLV8_VALUE_BOOLEAN && item->length == 1);
    TINY_TEST_CHECK(output != NULL && output->type == JSON_BOOLEAN);
    TINY_TEST_CHECK(output != NULL && JsonBoolean_GetValue(output->data.boolean));
    JsonValue_Delete(output);
    Tlv8Item_Delete(item);
    JsonValue_Delete(input);

    input = JsonValue_NewString("hello");
    item = Tlv8Item_NewFromJsonValue(input);
    output = (item != NULL) ? JsonValue_NewFromTlv8(item) : NULL;
    TINY_TEST_CHECK(item != NULL && item->type == TLV8_VALUE_STRING && item->length == 5);
    TINY_TEST_CHECK(output != NULL && output->type == JSON_STRING);
    TINY_TEST_CHECK(output != NULL && JsonString_GetLength(output->data.string) == 5);
    TINY_TEST_CHECK(output != NULL && memcmp(JsonString_GetValue(output->data.string), "hello", 5) == 0);
    JsonValue_Delete(output);
    Tlv8Item_Delete(item);
    JsonValue_Delete(input);

    /* JSON types without a TLV8 mapping are rejected. */
    input = JsonValue_NewNull();
    TINY_TEST_CHECK(Tlv8Item_NewFromJsonValue(input) == NULL);
    JsonValue_Delete(input);

    input = JsonValue_NewArray(JsonArray_New());
    TINY_TEST_CHECK(Tlv8Item_NewFromJsonValue(input) == NULL);
    JsonValue_Delete(input);

    input = JsonValue_NewObject(JsonObject_New());
    TINY_TEST_CHECK(Tlv8Item_NewFromJsonValue(input) == NULL);
    JsonValue_Delete(input);
}

static void test_item_arguments(void)
{
    Tlv8Item item;
    Tlv8Item broken;

    TINY_TEST_CHECK(Tlv8Item_Construct(NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Tlv8Item_Construct(&item) == TINY_RET_OK);

    TINY_TEST_CHECK(Tlv8Item_Set(NULL, 1, "a", 1) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Tlv8Item_Set(&item, 1, NULL, 1) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Tlv8Item_Append(NULL, "a", 1) == TINY_RET_E_NULL_PTR);

    Tlv8Item_Dispose(&item);

    Tlv8Item_Dispose(NULL);
    Tlv8Item_Delete(NULL);

    TINY_TEST_CHECK(Tlv8Item_Construct(&broken) == TINY_RET_OK);
    broken.length = 5; /* data is NULL: inconsistent on purpose */
    TINY_TEST_CHECK(JsonValue_NewFromTlv8(&broken) == NULL);
    Tlv8Item_Dispose(&broken);
}

static void test_list_heap_and_merge(void)
{
    Tlv8List *heap = Tlv8List_New();
    Tlv8List list;
    uint8_t parsed_bytes[3] = { 0x01, 0x01, 'd' };
    const uint8_t *bytes = NULL;

    TINY_TEST_CHECK(heap != NULL);
    if (heap != NULL)
    {
        TINY_TEST_CHECK(Tlv8List_AddByte(heap, 1, 0x41, false) == TINY_RET_OK);
        TINY_TEST_CHECK(Tlv8List_GetCount(heap) == 1);
        Tlv8List_Delete(heap);
    }

    TINY_TEST_CHECK(Tlv8List_Construct(&list) == TINY_RET_OK);

    TINY_TEST_CHECK(Tlv8List_AddBytes(NULL, 1, "a", 1, false) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Tlv8List_AddByte(NULL, 1, 1, false) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Tlv8List_AddSeparator(NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(Tlv8List_Get(NULL, 1) == NULL);
    TINY_TEST_CHECK(Tlv8List_GetAt(NULL, 0) == NULL);
    TINY_TEST_CHECK(Tlv8List_GetCount(NULL) == 0);
    TINY_TEST_CHECK(Tlv8List_GetBytes(NULL) == NULL);
    TINY_TEST_CHECK(Tlv8List_GetSize(NULL) == 0);
    Tlv8List_Clear(NULL);

    /* AddByte with merge appends to the existing item of the same type. */
    TINY_TEST_CHECK(Tlv8List_AddByte(&list, 1, 'a', false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_AddByte(&list, 2, 0xAA, false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_AddByte(&list, 2, 0xBB, true) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetCount(&list) == 2);
    TINY_TEST_CHECK(Tlv8List_Get(&list, 2) != NULL);
    TINY_TEST_CHECK(Tlv8List_Get(&list, 2)->length == 2);
    TINY_TEST_CHECK(Tlv8List_Get(&list, 2)->data[0] == 0xAA);
    TINY_TEST_CHECK(Tlv8List_Get(&list, 2)->data[1] == 0xBB);

    /* Without a matching item, merge still creates one. */
    TINY_TEST_CHECK(Tlv8List_AddByte(&list, 3, 0xCC, true) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetCount(&list) == 3);
    TINY_TEST_CHECK(Tlv8List_AddByte(&list, 0xFF, 1, false) == TINY_RET_E_PARAM_INVALID);

    /* Parsing appends to the existing items; merge extends them in place. */
    TINY_TEST_CHECK(Tlv8List_Parse(&list, parsed_bytes, sizeof(parsed_bytes), true) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetCount(&list) == 3);
    TINY_TEST_CHECK(Tlv8List_Get(&list, 1)->length == 2);
    TINY_TEST_CHECK(memcmp(Tlv8List_Get(&list, 1)->data, "ad", 2) == 0);

    bytes = Tlv8List_GetBytes(&list);
    TINY_TEST_CHECK(bytes != NULL);
    {
        uint32_t previous_size = Tlv8List_GetSize(&list);

        /* The serialization is cached until the list changes. */
        TINY_TEST_CHECK(Tlv8List_GetBytes(&list) == bytes);
        TINY_TEST_CHECK(Tlv8List_AddByte(&list, 4, 0x01, false) == TINY_RET_OK);
        TINY_TEST_CHECK(Tlv8List_GetSize(&list) == previous_size + 3U);
    }

    Tlv8List_Dispose(&list);
}

static void test_separator_round_trip(void)
{
    Tlv8List source;
    Tlv8List parsed;
    uint8_t separator[2] = { 0xFF, 0x00 };

    TINY_TEST_CHECK(Tlv8List_Construct(&source) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_AddSeparator(&source) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetSize(&source) == 2);
    TINY_TEST_CHECK(Tlv8List_GetBytes(&source)[0] == 0xFF);
    TINY_TEST_CHECK(Tlv8List_GetBytes(&source)[1] == 0x00);

    TINY_TEST_CHECK(Tlv8List_Construct(&parsed) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_Parse(&parsed, separator, sizeof(separator), false) == TINY_RET_OK);
    TINY_TEST_CHECK(Tlv8List_GetCount(&parsed) == 1);
    TINY_TEST_CHECK(Tlv8List_GetAt(&parsed, 0) != NULL);
    TINY_TEST_CHECK(Tlv8List_GetAt(&parsed, 0)->type == 0xFF);
    TINY_TEST_CHECK(Tlv8List_GetAt(&parsed, 0)->length == 0);
    TINY_TEST_CHECK(Tlv8List_GetSize(&parsed) == 2);
    TINY_TEST_CHECK(Tlv8List_GetBytes(&parsed)[0] == 0xFF);
    TINY_TEST_CHECK(Tlv8List_GetBytes(&parsed)[1] == 0x00);

    Tlv8List_Dispose(&parsed);
    Tlv8List_Dispose(&source);
}

static void test_factory_edge_types(void)
{
    Tlv8Item item;
    JsonValue *value = NULL;

    TINY_TEST_CHECK(Tlv8Item_NewFromJsonValue(NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewFromTlv8(NULL) == NULL);

    TINY_TEST_CHECK(Tlv8Item_Construct(&item) == TINY_RET_OK);

    /* Undefined and unknown wire types have no JSON mapping. */
    TINY_TEST_CHECK(Tlv8Item_Set(&item, (uint8_t)TLV8_VALUE_UNDEFINED, NULL, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonValue_NewFromTlv8(&item) == NULL);

    TINY_TEST_CHECK(Tlv8Item_Set(&item, 99, NULL, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonValue_NewFromTlv8(&item) == NULL);

    /* Bad payload lengths are rejected. */
    TINY_TEST_CHECK(Tlv8Item_Set(&item, (uint8_t)TLV8_VALUE_BOOLEAN, "ab", 2) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonValue_NewFromTlv8(&item) == NULL);

    TINY_TEST_CHECK(Tlv8Item_Set(&item, (uint8_t)TLV8_VALUE_INTEGER, "abc", 3) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonValue_NewFromTlv8(&item) == NULL);

    TINY_TEST_CHECK(Tlv8Item_Set(&item, (uint8_t)TLV8_VALUE_FLOAT, "abc", 3) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonValue_NewFromTlv8(&item) == NULL);

    /* An empty string is valid. */
    TINY_TEST_CHECK(Tlv8Item_Set(&item, (uint8_t)TLV8_VALUE_STRING, NULL, 0) == TINY_RET_OK);
    value = JsonValue_NewFromTlv8(&item);
    TINY_TEST_CHECK(value != NULL);
    TINY_TEST_CHECK(value != NULL && value->type == JSON_STRING);
    TINY_TEST_CHECK(value != NULL && JsonString_GetLength(value->data.string) == 0);
    JsonValue_Delete(value);

    Tlv8Item_Dispose(&item);
}

int main(void)
{
    TINY_TEST_BEGIN("tlv8");
    test_item();
    test_item_arguments();
    test_list_serialization();
    test_list_heap_and_merge();
    test_fragmentation();
    test_parse();
    test_separator_round_trip();
    test_factory_integers();
    test_factory_other_types();
    test_factory_edge_types();
    return TINY_TEST_FINISH();
}
