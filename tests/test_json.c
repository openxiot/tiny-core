/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <tinycore/json.h>

static JsonObject *parse_ok(const char *text)
{
    JsonDecoder decoder;
    JsonDecoderStatus status = JSON_DECODER_STATUS_INVALID;
    JsonObject *object = NULL;

    if (JsonDecoder_Construct(&decoder) != TINY_RET_OK)
    {
        return NULL;
    }

    object = JsonDecoder_Parse(&decoder, text, &status);
    JsonDecoder_Dispose(&decoder);

    if (status != JSON_DECODER_STATUS_OK)
    {
        JsonObject_Delete(object);
        return NULL;
    }

    return object;
}

static JsonDecoderStatus parse_status(const char *text)
{
    JsonDecoder decoder;
    JsonDecoderStatus status = JSON_DECODER_STATUS_OK;
    JsonObject *object = NULL;

    if (JsonDecoder_Construct(&decoder) != TINY_RET_OK)
    {
        return JSON_DECODER_STATUS_INVALID;
    }

    object = JsonDecoder_Parse(&decoder, text, &status);
    JsonObject_Delete(object);
    JsonDecoder_Dispose(&decoder);

    return status;
}

static void test_build_and_encode(void)
{
    JsonObject *object = JsonObject_New();
    JsonObject *nested = JsonObject_New();
    JsonArray *array = JsonArray_New();
    char *text = NULL;

    TINY_TEST_CHECK(object != NULL);
    if (object == NULL)
    {
        return;
    }

    TINY_TEST_CHECK(JsonObject_PutString(object, "name", "tiny") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutInteger(object, "count", 42) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutFloat(object, "ratio", 0.5) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutBoolean(object, "ok", true) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutNull(object, "none") == TINY_RET_OK);

    TINY_TEST_CHECK(nested != NULL && array != NULL);
    if (nested != NULL && array != NULL)
    {
        TINY_TEST_CHECK(JsonObject_PutString(nested, "x", "y") == TINY_RET_OK);
        TINY_TEST_CHECK(JsonArray_AddInteger(array, 1) == TINY_RET_OK);
        TINY_TEST_CHECK(JsonArray_AddInteger(array, 2) == TINY_RET_OK);
        TINY_TEST_CHECK(JsonArray_AddInteger(array, 3) == TINY_RET_OK);
        TINY_TEST_CHECK(JsonObject_PutArray(object, "list", array) == TINY_RET_OK);
        TINY_TEST_CHECK(JsonObject_PutObject(object, "nested", nested) == TINY_RET_OK);
    }
    else
    {
        JsonObject_Delete(nested);
        JsonArray_Delete(array);
    }

    TINY_TEST_CHECK(JsonObject_GetSize(object) == 7);
    TINY_TEST_CHECK(JsonObject_ContainsKey(object, "name"));
    TINY_TEST_CHECK(!JsonObject_ContainsKey(object, "missing"));
    TINY_TEST_CHECK(strcmp(JsonObject_GetStringValue(object, "name"), "tiny") == 0);
    TINY_TEST_CHECK(JsonObject_GetString(object, "name") != NULL);
    TINY_TEST_CHECK(JsonString_GetLength(JsonObject_GetString(object, "name")) == 4);
    TINY_TEST_CHECK(JsonObject_GetNumber(object, "count") != NULL);
    TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(object, "count")) == 42);
    TINY_TEST_CHECK(JsonObject_GetBoolean(object, "ok") != NULL);
    TINY_TEST_CHECK(JsonBoolean_GetValue(JsonObject_GetBoolean(object, "ok")));
    TINY_TEST_CHECK(JsonObject_GetArray(object, "list") != NULL);
    TINY_TEST_CHECK(JsonArray_GetSize(JsonObject_GetArray(object, "list")) == 3);
    TINY_TEST_CHECK(JsonArray_GetValue(JsonObject_GetArray(object, "list"), 2) != NULL);
    TINY_TEST_CHECK(JsonObject_GetObject(object, "nested") != NULL);

    /* Replacing a key keeps the insertion position and size. */
    TINY_TEST_CHECK(JsonObject_PutInteger(object, "count", 7) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 7);
    TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(object, "count")) == 7);

    TINY_TEST_CHECK(JsonObject_Remove(object, "none") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_Remove(object, "none") == TINY_RET_E_NOT_FOUND);
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 6);

    text = JsonObject_ToString(object, false);
    TINY_TEST_CHECK(text != NULL);
    if (text != NULL)
    {
        TINY_TEST_CHECK(strcmp(text,
            "{\"name\":\"tiny\",\"count\":7,\"ratio\":0.500000,\"ok\":true,"
            "\"list\":[1,2,3],\"nested\":{\"x\":\"y\"}}") == 0);
        free(text);
    }

    text = JsonObject_ToString(object, true);
    TINY_TEST_CHECK(text != NULL);
    if (text != NULL)
    {
        TINY_TEST_CHECK(strcmp(text,
            "{\n"
            "    \"name\": \"tiny\",\n"
            "    \"count\": 7,\n"
            "    \"ratio\": 0.500000,\n"
            "    \"ok\": true,\n"
            "    \"list\": [\n"
            "        1,\n"
            "        2,\n"
            "        3\n"
            "    ],\n"
            "    \"nested\": {\n"
            "        \"x\": \"y\"\n"
            "    }\n"
            "}") == 0);
        free(text);
    }

    JsonObject_Delete(object);
}

static void test_equality_and_copy(void)
{
    JsonObject *a = parse_ok("{\"x\":1,\"y\":[1,2],\"z\":{\"k\":true}}");
    JsonObject *b = parse_ok("{\"z\":{\"k\":true},\"y\":[1,2],\"x\":1}");
    JsonObject *copy = NULL;

    TINY_TEST_CHECK(a != NULL && b != NULL);
    if (a == NULL || b == NULL)
    {
        JsonObject_Delete(a);
        JsonObject_Delete(b);
        return;
    }

    TINY_TEST_CHECK(JsonObject_Equals(a, b));
    TINY_TEST_CHECK(!JsonObject_Equals(a, NULL));

    copy = JsonObject_NewFrom(a);
    TINY_TEST_CHECK(copy != NULL);
    TINY_TEST_CHECK(copy != NULL && JsonObject_Equals(copy, a));

    TINY_TEST_CHECK(JsonObject_Remove(a, "x") == TINY_RET_OK);
    TINY_TEST_CHECK(copy != NULL && JsonObject_ContainsKey(copy, "x"));
    TINY_TEST_CHECK(!JsonObject_Equals(a, copy));

    TINY_TEST_CHECK(b != NULL && JsonObject_GetSize(b) == 3);

    JsonObject_Delete(copy);
    JsonObject_Delete(a);
    JsonObject_Delete(b);
}

static void test_strings_and_escapes(void)
{
    JsonObject *object = JsonObject_New();
    JsonString *embedded = JsonString_NewLength("a\0b", 3);
    JsonValue *value = NULL;
    char *text = NULL;

    TINY_TEST_CHECK(object != NULL);
    if (object == NULL)
    {
        JsonString_Delete(embedded);
        return;
    }

    TINY_TEST_CHECK(JsonObject_PutString(object, "s", "quote\" back\\ slash/ tab\t\n\x01") == TINY_RET_OK);

    TINY_TEST_CHECK(embedded != NULL);
    if (embedded != NULL)
    {
        value = JsonValue_NewString(JsonString_GetValue(embedded));
        TINY_TEST_CHECK(value == NULL || value->type == JSON_STRING);
        JsonValue_Delete(value);

        value = JsonValue_NewValue(JSON_STRING, embedded);
        TINY_TEST_CHECK(value != NULL);
        TINY_TEST_CHECK(value != NULL && JsonString_GetLength(value->data.string) == 3);
        TINY_TEST_CHECK(JsonObject_PutValue(object, "nul", value) == TINY_RET_OK);
    }

    text = JsonObject_ToString(object, false);
    TINY_TEST_CHECK(text != NULL);
    if (text != NULL)
    {
        TINY_TEST_CHECK(strcmp(text,
            "{\"s\":\"quote\\\" back\\\\ slash/ tab\\t\\n\\u0001\","
            "\"nul\":\"a\\u0000b\"}") == 0);
        free(text);
    }

    {
        JsonObject *decoded = parse_ok("{\"s\":\"quote\\\" back\\\\ slash/ tab\\t\\n\\u0001\","
                                       "\"nul\":\"a\\u0000b\"}");
        TINY_TEST_CHECK(decoded != NULL);
        if (decoded != NULL)
        {
            JsonString *s = JsonObject_GetString(decoded, "nul");
            TINY_TEST_CHECK(s != NULL);
            TINY_TEST_CHECK(s != NULL && JsonString_GetLength(s) == 3);
            TINY_TEST_CHECK(s != NULL && memcmp(JsonString_GetValue(s), "a\0b", 3) == 0);
            TINY_TEST_CHECK(JsonObject_Equals(decoded, object));
            JsonObject_Delete(decoded);
        }
    }

    JsonObject_Delete(object);
}

static void test_numbers(void)
{
    JsonObject *object = parse_ok("{\"i\":-2147483648,\"f\":1.5,\"e\":1e3,\"big\":2147483648,"
                                  "\"zero\":0,\"neg\":-0.25}");
    char *text = NULL;

    TINY_TEST_CHECK(object != NULL);
    if (object == NULL)
    {
        return;
    }

    TINY_TEST_CHECK(JsonNumber_IsInteger(JsonObject_GetNumber(object, "i")));
    TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(object, "i")) == (-2147483647 - 1));
    TINY_TEST_CHECK(!JsonNumber_IsInteger(JsonObject_GetNumber(object, "f")));
    TINY_TEST_CHECK(JsonNumber_GetFloat(JsonObject_GetNumber(object, "f")) == 1.5);
    TINY_TEST_CHECK(JsonNumber_GetFloat(JsonObject_GetNumber(object, "e")) == 1000.0);
    TINY_TEST_CHECK(!JsonNumber_IsInteger(JsonObject_GetNumber(object, "big")));
    TINY_TEST_CHECK(JsonNumber_GetFloat(JsonObject_GetNumber(object, "big")) == 2147483648.0);
    TINY_TEST_CHECK(JsonNumber_IsInteger(JsonObject_GetNumber(object, "zero")));
    TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(object, "zero")) == 0);
    TINY_TEST_CHECK(JsonNumber_GetFloat(JsonObject_GetNumber(object, "neg")) == -0.25);

    text = JsonObject_ToString(object, false);
    TINY_TEST_CHECK(text != NULL);
    if (text != NULL)
    {
        TINY_TEST_CHECK(strcmp(text,
            "{\"i\":-2147483648,\"f\":1.500000,\"e\":1000.000000,"
            "\"big\":2147483648.000000,\"zero\":0,\"neg\":-0.250000}") == 0);
        free(text);
    }

    /* Non-finite values have no JSON form and encode as null. */
    {
        JsonObject *special = JsonObject_New();
        TINY_TEST_CHECK(special != NULL);
        if (special != NULL)
        {
            TINY_TEST_CHECK(JsonObject_PutFloat(special, "inf", INFINITY) == TINY_RET_OK);
            TINY_TEST_CHECK(JsonObject_PutFloat(special, "nan", NAN) == TINY_RET_OK);
            text = JsonObject_ToString(special, false);
            TINY_TEST_CHECK(text != NULL && strcmp(text, "{\"inf\":null,\"nan\":null}") == 0);
            free(text);
            JsonObject_Delete(special);
        }
    }

    JsonObject_Delete(object);
}

static void test_unicode(void)
{
    JsonObject *object = parse_ok("{\"emoji\":\"\\ud83d\\ude00\",\"lang\":\"\\u8bed\\u8a00\"}");
    JsonString *emoji = NULL;
    JsonString *lang = NULL;

    TINY_TEST_CHECK(object != NULL);
    if (object == NULL)
    {
        return;
    }

    emoji = JsonObject_GetString(object, "emoji");
    TINY_TEST_CHECK(emoji != NULL);
    TINY_TEST_CHECK(emoji != NULL && JsonString_GetLength(emoji) == 4);
    TINY_TEST_CHECK(emoji != NULL && memcmp(JsonString_GetValue(emoji), "\xF0\x9F\x98\x80", 4) == 0);

    lang = JsonObject_GetString(object, "lang");
    TINY_TEST_CHECK(lang != NULL);
    TINY_TEST_CHECK(lang != NULL && JsonString_GetLength(lang) == 6);
    TINY_TEST_CHECK(lang != NULL && memcmp(JsonString_GetValue(lang), "\xE8\xAF\xAD\xE8\xA8\x80", 6) == 0);

    TINY_TEST_CHECK(parse_status("{\"a\":\"\\ud800\"}") == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(parse_status("{\"a\":\"\\udc00\"}") == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(parse_status("{\"a\":\"\\ud800x\"}") == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(parse_status("{\"\\u0000\":1}") == JSON_DECODER_STATUS_INVALID);

    JsonObject_Delete(object);
}

static void test_invalid_documents(void)
{
    static const char *const invalid[] = {
        "}",
        "[]",
        "null",
        "1",
        "{\"a\":1} x",
        "{\"a\":1}{}",
        "{\"a\":01}",
        "{\"a\":1,}",
        "{\"a\" 1}",
        "{\"a\":}",
        "{\"a\":[1,2,]}",
        "{\"a\":{\"b\":}}",
        "{\"a\":truex}",
        "{\"a\":1.}",
        "{\"a\":.5}",
        "{a:1}",
        "{\"a\":1,,\"b\":2}"
    };
    size_t i = 0;

    for (i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
    {
        TINY_TEST_CHECK_MSG(parse_status(invalid[i]) == JSON_DECODER_STATUS_INVALID,
                            "input: %s", invalid[i]);
    }

    TINY_TEST_CHECK(parse_status("{\"a\":") == JSON_DECODER_STATUS_INCOMPLETE);
    TINY_TEST_CHECK(parse_status("{\"a\":[1,2") == JSON_DECODER_STATUS_INCOMPLETE);
    TINY_TEST_CHECK(parse_status("{\"a\":\"x") == JSON_DECODER_STATUS_INCOMPLETE);
    TINY_TEST_CHECK(parse_status("{") == JSON_DECODER_STATUS_INCOMPLETE);
    TINY_TEST_CHECK(parse_status("{\"a\":\"unterminated}") == JSON_DECODER_STATUS_INCOMPLETE);
    TINY_TEST_CHECK(parse_status("") == JSON_DECODER_STATUS_INCOMPLETE);
    TINY_TEST_CHECK(parse_status("  ") == JSON_DECODER_STATUS_INCOMPLETE);
}

static void test_parse_edges(void)
{
    JsonObject *object = NULL;
    char document[256];
    char *cursor = NULL;
    size_t i = 0;

    /* Escapes rejected at the token level. */
    TINY_TEST_CHECK(parse_status("{\"a\":\"\\x\"}") == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(parse_status("{\"a\":\"\\u12\"}") == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(parse_status("{\"a\":\"\\u12G4\"}") == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(parse_status("{\"a\":\"x\ty\"}") == JSON_DECODER_STATUS_INVALID);

    /* Numbers longer than the decoder's fixed buffer are rejected. */
    cursor = document;
    memcpy(cursor, "{\"a\":", 5);
    cursor += 5;
    memset(cursor, '9', 128);
    cursor += 128;
    *cursor++ = '}';
    *cursor = '\0';
    TINY_TEST_CHECK(parse_status(document) == JSON_DECODER_STATUS_INVALID);

    /* One byte below the limit the number parses as a float. */
    cursor = document;
    memcpy(cursor, "{\"a\":", 5);
    cursor += 5;
    memset(cursor, '1', 127);
    cursor += 127;
    *cursor++ = '}';
    *cursor = '\0';
    object = parse_ok(document);
    TINY_TEST_CHECK(object != NULL);
    TINY_TEST_CHECK(object != NULL && JsonObject_GetNumber(object, "a") != NULL);
    TINY_TEST_CHECK(object != NULL && JsonObject_GetNumber(object, "a") != NULL &&
                    !JsonNumber_IsInteger(JsonObject_GetNumber(object, "a")));
    JsonObject_Delete(object);

    /* Values outside the int32 range and floats beyond double fall back. */
    object = parse_ok("{\"overflow\":1e309,\"below\":-2147483649}");
    TINY_TEST_CHECK(object != NULL);
    if (object != NULL)
    {
        char *text = NULL;

        TINY_TEST_CHECK(!JsonNumber_IsInteger(JsonObject_GetNumber(object, "overflow")));
        TINY_TEST_CHECK(isinf(JsonNumber_GetFloat(JsonObject_GetNumber(object, "overflow"))));
        TINY_TEST_CHECK(!JsonNumber_IsInteger(JsonObject_GetNumber(object, "below")));
        TINY_TEST_CHECK(JsonNumber_GetFloat(JsonObject_GetNumber(object, "below")) ==
                        -2147483649.0);

        text = JsonObject_ToString(object, false);
        TINY_TEST_CHECK(text != NULL);
        TINY_TEST_CHECK(text != NULL &&
                        strcmp(text, "{\"overflow\":null,\"below\":-2147483649.000000}") == 0);
        free(text);
        JsonObject_Delete(object);
    }

    /* Empty keys are valid; duplicate keys keep the last value. */
    TINY_TEST_CHECK(parse_status("{\"\":1}") == JSON_DECODER_STATUS_OK);
    object = parse_ok("{\"a\":1,\"a\":2}");
    TINY_TEST_CHECK(object != NULL);
    TINY_TEST_CHECK(object != NULL && JsonObject_GetSize(object) == 1);
    TINY_TEST_CHECK(object != NULL &&
                    JsonNumber_GetInteger(JsonObject_GetNumber(object, "a")) == 2);
    JsonObject_Delete(object);

    /* Deep but bounded nesting. */
    cursor = document;
    for (i = 0; i < 32; ++i)
    {
        memcpy(cursor, "{\"a\":", 5);
        cursor += 5;
    }
    *cursor++ = '1';
    for (i = 0; i < 32; ++i)
    {
        *cursor++ = '}';
    }
    *cursor = '\0';

    object = parse_ok(document);
    TINY_TEST_CHECK(object != NULL);
    if (object != NULL)
    {
        JsonObject *level = object;

        for (i = 0; i < 32 && level != NULL; ++i)
        {
            JsonObject *next = JsonObject_GetObject(level, "a");

            if (i == 31)
            {
                TINY_TEST_CHECK(next == NULL);
                TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(level, "a")) == 1);
            }
            else
            {
                TINY_TEST_CHECK(next != NULL);
            }

            level = next;
        }

        JsonObject_Delete(object);
    }

    /* A long string value is preserved byte for byte. */
    {
        char long_document[4200];
        size_t prefix = strlen("{\"s\":\"");
        size_t suffix = strlen("\"}");

        memcpy(long_document, "{\"s\":\"", prefix);
        memset(long_document + prefix, 'a', 4000);
        memcpy(long_document + prefix + 4000, "\"}", suffix + 1U);

        object = parse_ok(long_document);
        TINY_TEST_CHECK(object != NULL);
        TINY_TEST_CHECK(object != NULL &&
                        JsonString_GetLength(JsonObject_GetString(object, "s")) == 4000);
        JsonObject_Delete(object);
    }

    /* Control characters and the solidus round trip through encoding. */
    object = parse_ok("{\"a\":\"a\\/b\\b\\f\\n\\r\\t\\u0001\"}");
    TINY_TEST_CHECK(object != NULL);
    if (object != NULL)
    {
        char *text = JsonObject_ToString(object, false);

        TINY_TEST_CHECK(text != NULL);
        TINY_TEST_CHECK(text != NULL &&
                        strcmp(text, "{\"a\":\"a/b\\b\\f\\n\\r\\t\\u0001\"}") == 0);
        free(text);
        JsonObject_Delete(object);
    }

    /* An escaped NUL is allowed inside values but not inside keys. */
    object = parse_ok("{\"a\":\"\\u0000\"}");
    TINY_TEST_CHECK(object != NULL);
    TINY_TEST_CHECK(object != NULL &&
                    JsonString_GetLength(JsonObject_GetString(object, "a")) == 1);
    TINY_TEST_CHECK(object != NULL &&
                    JsonString_GetValue(JsonObject_GetString(object, "a"))[0] == '\0');
    JsonObject_Delete(object);
    TINY_TEST_CHECK(parse_status("{\"\\u0000\":1}") == JSON_DECODER_STATUS_INVALID);

    /* Empty containers and whitespace tolerance. */
    object = parse_ok("\r\n\t {\"a\" : [ ] , \"b\" : { } , \"c\" : [ [ ] ] } \n");
    TINY_TEST_CHECK(object != NULL);
    if (object != NULL)
    {
        TINY_TEST_CHECK(JsonObject_GetSize(object) == 3);
        TINY_TEST_CHECK(JsonObject_GetArray(object, "a") != NULL);
        TINY_TEST_CHECK(JsonArray_GetSize(JsonObject_GetArray(object, "a")) == 0);
        TINY_TEST_CHECK(JsonObject_GetObject(object, "b") != NULL);
        TINY_TEST_CHECK(JsonObject_GetSize(JsonObject_GetObject(object, "b")) == 0);
        TINY_TEST_CHECK(JsonObject_GetArray(object, "c") != NULL);
        TINY_TEST_CHECK(JsonArray_GetSize(JsonObject_GetArray(object, "c")) == 1);
        TINY_TEST_CHECK(JsonArray_GetArray(JsonObject_GetArray(object, "c"), 0) != NULL);
        TINY_TEST_CHECK(JsonArray_GetSize(JsonArray_GetArray(JsonObject_GetArray(object, "c"), 0)) == 0);
        JsonObject_Delete(object);
    }

    object = JsonObject_NewString("{}");
    TINY_TEST_CHECK(object != NULL && JsonObject_IsEmpty(object));
    if (object != NULL)
    {
        char *text = JsonObject_ToString(object, false);

        TINY_TEST_CHECK(text != NULL && strcmp(text, "{}") == 0);
        free(text);
        JsonObject_Delete(object);
    }
}

static void test_parse_helpers(void)
{
    JsonObject *object = JsonObject_NewString("{\"a\":1}");
    JsonObject *fast = JsonObject_NewStringFast("{\"a\":1}");

    TINY_TEST_CHECK(object != NULL);
    TINY_TEST_CHECK(fast != NULL);
    TINY_TEST_CHECK(object != NULL && JsonObject_GetSize(object) == 1);
    TINY_TEST_CHECK(fast != NULL && JsonObject_GetSize(fast) == 1);
    TINY_TEST_CHECK(JsonObject_NewString("not json") == NULL);
    TINY_TEST_CHECK(JsonObject_NewString(NULL) == NULL);

    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_OBJECT), "object") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_ARRAY), "array") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_UNDEFINED), "undefined") == 0);

    JsonObject_Delete(object);
    JsonObject_Delete(fast);
}

static void test_incremental_feed(void)
{
    JsonDecoder decoder;
    JsonDecoderStatus status = JSON_DECODER_STATUS_INVALID;
    JsonObject *object = NULL;

    TINY_TEST_CHECK(JsonDecoder_Construct(&decoder) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonDecoder_Feed(&decoder, "{\"a\":", 5) == TINY_RET_OK);
    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(object == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INCOMPLETE);

    TINY_TEST_CHECK(JsonDecoder_Feed(&decoder, "1}", 2) == TINY_RET_OK);
    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(object != NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_OK);
    if (object != NULL)
    {
        TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(object, "a")) == 1);
    }
    JsonObject_Delete(object);

    /* Two documents fed at once are decoded one at a time. */
    TINY_TEST_CHECK(JsonDecoder_Feed(&decoder, "{\"b\":2}{\"c\":3}", 14) == TINY_RET_OK);

    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(object != NULL && status == JSON_DECODER_STATUS_OK);
    TINY_TEST_CHECK(object != NULL && JsonObject_ContainsKey(object, "b"));
    JsonObject_Delete(object);

    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(object != NULL && status == JSON_DECODER_STATUS_OK);
    TINY_TEST_CHECK(object != NULL && JsonObject_ContainsKey(object, "c"));
    JsonObject_Delete(object);

    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(object == NULL && status == JSON_DECODER_STATUS_INCOMPLETE);

    /* A complete invalid document is reported and discarded. */
    TINY_TEST_CHECK(JsonDecoder_Feed(&decoder, "{\"a\":}", 6) == TINY_RET_OK);
    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(object == NULL && status == JSON_DECODER_STATUS_INVALID);

    TINY_TEST_CHECK(JsonDecoder_Feed(NULL, "{}", 2) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonDecoder_Feed(&decoder, NULL, 0) == TINY_RET_OK);

    JsonDecoder_Dispose(&decoder);
}

static void test_cycle_buffer_input(void)
{
    JsonDecoder decoder;
    JsonDecoderStatus status = JSON_DECODER_STATUS_INVALID;
    CycleBuffer buffer;
    JsonObject *object = NULL;
    const char *documents = "{\"b\":2}{\"c\":3}";

    TINY_TEST_CHECK(JsonDecoder_Construct(&decoder) == TINY_RET_OK);
    TINY_TEST_CHECK(CycleBuffer_Construct(&buffer, 64) == TINY_RET_OK);

    /* Partial document: nothing is consumed until it is complete. */
    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, documents, 6) == 6);
    object = JsonDecoder_ParseBuffer(&decoder, &buffer, &status);
    TINY_TEST_CHECK(object == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INCOMPLETE);

    TINY_TEST_CHECK(CycleBuffer_Write(&buffer, documents + 6, strlen(documents) - 6) == strlen(documents) - 6);

    object = JsonDecoder_ParseBuffer(&decoder, &buffer, &status);
    TINY_TEST_CHECK(object != NULL && status == JSON_DECODER_STATUS_OK);
    TINY_TEST_CHECK(object != NULL && JsonObject_ContainsKey(object, "b"));
    JsonObject_Delete(object);

    object = JsonDecoder_ParseBuffer(&decoder, &buffer, &status);
    TINY_TEST_CHECK(object != NULL && status == JSON_DECODER_STATUS_OK);
    TINY_TEST_CHECK(object != NULL && JsonObject_ContainsKey(object, "c"));
    JsonObject_Delete(object);

    TINY_TEST_CHECK(CycleBuffer_IsEmpty(&buffer));
    object = JsonDecoder_ParseBuffer(&decoder, &buffer, &status);
    TINY_TEST_CHECK(object == NULL && status == JSON_DECODER_STATUS_INCOMPLETE);

    CycleBuffer_Dispose(&buffer);
    JsonDecoder_Dispose(&decoder);
}

int main(void)
{
    TINY_TEST_BEGIN("json");
    test_build_and_encode();
    test_equality_and_copy();
    test_strings_and_escapes();
    test_numbers();
    test_unicode();
    test_invalid_documents();
    test_parse_edges();
    test_parse_helpers();
    test_incremental_feed();
    test_cycle_buffer_input();
    return TINY_TEST_FINISH();
}
