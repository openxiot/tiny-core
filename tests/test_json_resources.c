/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tinycore/json.h>

#ifndef TINYCORE_TEST_RESOURCE_DIR
#  error "TINYCORE_TEST_RESOURCE_DIR must be defined by the build system"
#endif

static const char *const BUFFER_RESOURCES[] = {
    "buffer/00.json", "buffer/11.json", "buffer/12.json", "buffer/21.json",
    "buffer/22.json", "buffer/31.json", "buffer/32.json", "buffer/41.json",
    "buffer/42.json", "buffer/43.json", "buffer/44.json", "buffer/51.json",
    "buffer/52.json"
};

static const char *const STRING_RESOURCES[] = {
    "string/00.json", "string/100.json", "string/11.json", "string/12.json",
    "string/21.json", "string/22.json", "string/31.json", "string/32.json",
    "string/41.json", "string/42.json", "string/43.json", "string/44.json",
    "string/51.json", "string/52.json", "string/owner-event-occurred.json"
};

static const char *const FLOAT_RESOURCES[] = {
    "float/float-01.json", "float/float-02.json", "float/float-03.json",
    "float/float-04.json", "float/float-05.json"
};

static const char *const INVALID_RESOURCES[] = {
    "invalid/00.json", "invalid/100.json", "invalid/11.json", "invalid/12.json",
    "invalid/21.json", "invalid/22.json", "invalid/31.json", "invalid/32.json",
    "invalid/41.json", "invalid/42.json", "invalid/43.json", "invalid/44.json",
    "invalid/51.json", "invalid/52.json"
};

static const char *const PART_RESOURCES[] = {
    "part/00.json", "part/01.json", "part/02.json", "part/03.json"
};

/* Expected number of complete documents in each part file; each file ends
 * with either nothing or an incomplete document. */
static const uint32_t PART_EXPECTED[] = { 4, 4, 5, 4 };

/* Status left in the decoder after the last complete document: part/02 ends
 * with a stray closing brace. */
static const JsonDecoderStatus PART_TERMINAL_STATUS[] = {
    JSON_DECODER_STATUS_INCOMPLETE,
    JSON_DECODER_STATUS_INCOMPLETE,
    JSON_DECODER_STATUS_INVALID,
    JSON_DECODER_STATUS_INCOMPLETE
};

static const char *const EQUALS_RESOURCES[] = {
    "equals/1", "equals/2", "equals/3", "equals/4",
    "equals/5", "equals/6", "equals/7", "equals/8"
};

static char *read_resource(const char *name, size_t *length)
{
    char path[1024];
    FILE *file = NULL;
    long size = 0;
    char *data = NULL;
    size_t count = 0;

    if (length != NULL)
    {
        *length = 0;
    }

    {
        int written = snprintf(path, sizeof(path), "%s/%s", TINYCORE_TEST_RESOURCE_DIR, name);

        if (written < 0 || (size_t)written >= sizeof(path))
        {
            return NULL;
        }
    }

    file = fopen(path, "rb");
    if (file == NULL)
    {
        return NULL;
    }

    if (fseek(file, 0L, SEEK_END) != 0)
    {
        fclose(file);
        return NULL;
    }

    size = ftell(file);
    if (size < 0 || fseek(file, 0L, SEEK_SET) != 0)
    {
        fclose(file);
        return NULL;
    }

    data = (char *)malloc((size_t)size + 1U);
    if (data == NULL)
    {
        fclose(file);
        return NULL;
    }

    count = fread(data, 1U, (size_t)size, file);
    fclose(file);

    if (count != (size_t)size)
    {
        free(data);
        return NULL;
    }

    data[count] = '\0';
    if (length != NULL)
    {
        *length = count;
    }

    return data;
}

static JsonObject *decode_file(const char *name)
{
    size_t length = 0;
    char *text = read_resource(name, &length);
    JsonDecoder decoder;
    JsonDecoderStatus status = JSON_DECODER_STATUS_INVALID;
    JsonObject *object = NULL;

    if (text == NULL)
    {
        return NULL;
    }

    if (JsonDecoder_Construct(&decoder) == TINY_RET_OK)
    {
        object = JsonDecoder_Parse(&decoder, text, &status);
        JsonDecoder_Dispose(&decoder);
    }

    free(text);
    return (status == JSON_DECODER_STATUS_OK) ? object : NULL;
}

/* Count the complete documents in a resource, feeding the decoder one byte at
 * a time when byte_by_byte is true.  Returns UINT32_MAX on I/O failure. */
static uint32_t count_documents(const char *name,
                                bool byte_by_byte,
                                JsonDecoderStatus expected_status)
{
    size_t length = 0;
    char *text = read_resource(name, &length);
    JsonDecoder decoder;
    JsonDecoderStatus status = JSON_DECODER_STATUS_INCOMPLETE;
    uint32_t count = 0;
    size_t i = 0;

    if (text == NULL)
    {
        return UINT32_MAX;
    }

    if (JsonDecoder_Construct(&decoder) != TINY_RET_OK)
    {
        free(text);
        return UINT32_MAX;
    }

    if (byte_by_byte)
    {
        for (i = 0; i < length; ++i)
        {
            if (JsonDecoder_Feed(&decoder, text + i, 1U) != TINY_RET_OK)
            {
                JsonDecoder_Dispose(&decoder);
                free(text);
                return UINT32_MAX;
            }
        }
    }
    else if (JsonDecoder_Feed(&decoder, text, length) != TINY_RET_OK)
    {
        JsonDecoder_Dispose(&decoder);
        free(text);
        return UINT32_MAX;
    }

    for (;;)
    {
        JsonObject *object = JsonDecoder_ParseBuffered(&decoder, &status);

        if (object == NULL)
        {
            break;
        }

        ++count;
        JsonObject_Delete(object);

        if (status != JSON_DECODER_STATUS_OK)
        {
            break;
        }
    }

    JsonDecoder_Dispose(&decoder);
    free(text);

    TINY_TEST_CHECK_MSG(status == expected_status,
                        "%s: terminal status %d", name, (int)status);

    return count;
}

static void test_buffer_resources(void)
{
    size_t i = 0;

    for (i = 0; i < sizeof(BUFFER_RESOURCES) / sizeof(BUFFER_RESOURCES[0]); ++i)
    {
        size_t length = 0;
        char *text = read_resource(BUFFER_RESOURCES[i], &length);
        JsonObject *object = decode_file(BUFFER_RESOURCES[i]);
        char *encoded = NULL;

        TINY_TEST_CHECK_MSG(text != NULL, "missing %s", BUFFER_RESOURCES[i]);
        TINY_TEST_CHECK_MSG(object != NULL, "cannot decode %s", BUFFER_RESOURCES[i]);

        if (text != NULL && object != NULL)
        {
            encoded = JsonObject_ToString(object, false);
            TINY_TEST_CHECK_MSG(encoded != NULL, "cannot encode %s", BUFFER_RESOURCES[i]);
            TINY_TEST_CHECK_MSG(encoded != NULL && strcmp(encoded, text) == 0,
                                "round trip mismatch: %s", BUFFER_RESOURCES[i]);

            /* The incremental decoder must accept the same document. */
            {
                JsonDecoder decoder;
                CycleBuffer buffer;
                JsonDecoderStatus status = JSON_DECODER_STATUS_INVALID;
                JsonObject *incremental = NULL;

                TINY_TEST_CHECK(JsonDecoder_Construct(&decoder) == TINY_RET_OK);
                TINY_TEST_CHECK(CycleBuffer_Construct(&buffer, length + 1U) == TINY_RET_OK);
                TINY_TEST_CHECK(CycleBuffer_Write(&buffer, text, length) == length);
                incremental = JsonDecoder_ParseBuffer(&decoder, &buffer, &status);
                TINY_TEST_CHECK(incremental != NULL);
                TINY_TEST_CHECK(status == JSON_DECODER_STATUS_OK);
                TINY_TEST_CHECK(incremental != NULL && JsonObject_Equals(incremental, object));

                JsonObject_Delete(incremental);
                CycleBuffer_Dispose(&buffer);
                JsonDecoder_Dispose(&decoder);
            }
        }

        free(encoded);
        JsonObject_Delete(object);
        free(text);
    }
}

static void test_string_resources(void)
{
    size_t i = 0;

    for (i = 0; i < sizeof(STRING_RESOURCES) / sizeof(STRING_RESOURCES[0]); ++i)
    {
        JsonObject *object = decode_file(STRING_RESOURCES[i]);
        char *compact = NULL;
        JsonObject *round_trip = NULL;

        TINY_TEST_CHECK_MSG(object != NULL, "cannot decode %s", STRING_RESOURCES[i]);
        if (object == NULL)
        {
            continue;
        }

        compact = JsonObject_ToString(object, false);
        TINY_TEST_CHECK_MSG(compact != NULL, "cannot encode %s", STRING_RESOURCES[i]);
        if (compact != NULL)
        {
            round_trip = JsonObject_NewString(compact);
            TINY_TEST_CHECK_MSG(round_trip != NULL, "cannot reparse %s", STRING_RESOURCES[i]);
            TINY_TEST_CHECK(round_trip != NULL && JsonObject_Equals(object, round_trip));
            JsonObject_Delete(round_trip);
        }

        free(compact);

        {
            char *pretty = JsonObject_ToString(object, true);
            JsonObject *pretty_parsed = NULL;

            TINY_TEST_CHECK(pretty != NULL);
            if (pretty != NULL)
            {
                pretty_parsed = JsonObject_NewString(pretty);
                TINY_TEST_CHECK(pretty_parsed != NULL);
                TINY_TEST_CHECK(pretty_parsed != NULL && JsonObject_Equals(object, pretty_parsed));
                JsonObject_Delete(pretty_parsed);
            }
            free(pretty);
        }

        JsonObject_Delete(object);
    }
}

static void test_float_resources(void)
{
    static const double expected[] = { 0.12345, 0.00001, 0.00009, 0.9999, 1.01 };
    size_t i = 0;

    for (i = 0; i < sizeof(FLOAT_RESOURCES) / sizeof(FLOAT_RESOURCES[0]); ++i)
    {
        JsonObject *object = decode_file(FLOAT_RESOURCES[i]);
        JsonNumber *number = NULL;
        char *compact = NULL;

        TINY_TEST_CHECK_MSG(object != NULL, "cannot decode %s", FLOAT_RESOURCES[i]);
        if (object == NULL)
        {
            continue;
        }

        number = JsonObject_GetNumber(object, "value");
        TINY_TEST_CHECK_MSG(number != NULL, "missing value: %s", FLOAT_RESOURCES[i]);
        TINY_TEST_CHECK(number != NULL && !JsonNumber_IsInteger(number));
        TINY_TEST_CHECK(number != NULL && JsonNumber_GetFloat(number) > expected[i] - 1e-9);
        TINY_TEST_CHECK(number != NULL && JsonNumber_GetFloat(number) < expected[i] + 1e-9);

        compact = JsonObject_ToString(object, false);
        TINY_TEST_CHECK(compact != NULL && strncmp(compact, "{\"value\":", 9) == 0);

        free(compact);
        JsonObject_Delete(object);
    }
}

static void test_part_resources(void)
{
    size_t i = 0;

    for (i = 0; i < sizeof(PART_RESOURCES) / sizeof(PART_RESOURCES[0]); ++i)
    {
        TINY_TEST_CHECK_MSG(count_documents(PART_RESOURCES[i], false, PART_TERMINAL_STATUS[i]) == PART_EXPECTED[i],
                            "document count: %s", PART_RESOURCES[i]);
        TINY_TEST_CHECK_MSG(count_documents(PART_RESOURCES[i], true, PART_TERMINAL_STATUS[i]) == PART_EXPECTED[i],
                            "byte by byte document count: %s", PART_RESOURCES[i]);
    }

    /* A whole file with several documents is not a single document. */
    {
        size_t length = 0;
        char *text = read_resource("part/00.json", &length);
        JsonDecoder decoder;
        JsonDecoderStatus status = JSON_DECODER_STATUS_OK;
        JsonObject *object = NULL;

        TINY_TEST_CHECK(text != NULL);
        TINY_TEST_CHECK(JsonDecoder_Construct(&decoder) == TINY_RET_OK);
        object = (text != NULL) ? JsonDecoder_Parse(&decoder, text, &status) : NULL;
        TINY_TEST_CHECK(object == NULL);
        TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);
        JsonObject_Delete(object);
        JsonDecoder_Dispose(&decoder);
        free(text);
    }
}

static void test_equals_resources(void)
{
    size_t i = 0;

    for (i = 0; i < sizeof(EQUALS_RESOURCES) / sizeof(EQUALS_RESOURCES[0]); ++i)
    {
        char a_name[64];
        char b_name[64];
        JsonObject *a = NULL;
        JsonObject *b = NULL;

        snprintf(a_name, sizeof(a_name), "%s/a.json", EQUALS_RESOURCES[i]);
        snprintf(b_name, sizeof(b_name), "%s/b.json", EQUALS_RESOURCES[i]);

        a = decode_file(a_name);
        b = decode_file(b_name);

        TINY_TEST_CHECK_MSG(a != NULL && b != NULL, "cannot decode %s", EQUALS_RESOURCES[i]);
        TINY_TEST_CHECK_MSG(a != NULL && b != NULL && JsonObject_Equals(a, b),
                            "objects differ: %s", EQUALS_RESOURCES[i]);
        TINY_TEST_CHECK_MSG(a != NULL && b != NULL && JsonObject_Equals(b, a),
                            "equality is not symmetric: %s", EQUALS_RESOURCES[i]);

        JsonObject_Delete(a);
        JsonObject_Delete(b);
    }
}

static void test_invalid_resources(void)
{
    size_t i = 0;

    for (i = 0; i < sizeof(INVALID_RESOURCES) / sizeof(INVALID_RESOURCES[0]); ++i)
    {
        TINY_TEST_CHECK_MSG(decode_file(INVALID_RESOURCES[i]) == NULL,
                            "accepted invalid document: %s", INVALID_RESOURCES[i]);
    }
}

static void test_utf8_resource(void)
{
    JsonObject *object = decode_file("utf8/00.json");
    JsonString *key_string = NULL;
    JsonString *value_string = NULL;
    char *compact = NULL;

    TINY_TEST_CHECK(object != NULL);
    if (object == NULL)
    {
        return;
    }

    TINY_TEST_CHECK(JsonObject_ContainsKey(object, "\xE8\xAF\xAD\xE8\xA8\x80"));
    key_string = JsonObject_GetString(object, "\xE8\xAF\xAD\xE8\xA8\x80");
    value_string = JsonObject_GetString(object, "\xE8\xAF\xAD\xE8\xA8\x80");

    TINY_TEST_CHECK(key_string != NULL && value_string != NULL);
    TINY_TEST_CHECK(value_string != NULL && JsonString_GetLength(value_string) == 6);
    TINY_TEST_CHECK(value_string != NULL &&
                    memcmp(JsonString_GetValue(value_string), "\xE4\xB8\xAD\xE6\x96\x87", 6) == 0);

    compact = JsonObject_ToString(object, false);
    TINY_TEST_CHECK(compact != NULL && strcmp(compact, "{\"\xE8\xAF\xAD\xE8\xA8\x80\":\"\xE4\xB8\xAD\xE6\x96\x87\"}") == 0);
    free(compact);
    JsonObject_Delete(object);
}

static void test_owner_event_resource(void)
{
    JsonObject *object = decode_file("string/owner-event-occurred.json");
    JsonObject *message = NULL;
    JsonObject *payload = NULL;
    JsonObject *event = NULL;
    JsonArray *arguments = NULL;
    JsonObject *first = NULL;
    JsonArray *values = NULL;

    TINY_TEST_CHECK(object != NULL);
    if (object == NULL)
    {
        return;
    }

    message = JsonObject_GetObject(object, "message");
    TINY_TEST_CHECK(message != NULL);
    if (message == NULL)
    {
        JsonObject_Delete(object);
        return;
    }

    TINY_TEST_CHECK(strcmp(JsonObject_GetStringValue(message, "topic"), "urn:xiot:owner") == 0);
    TINY_TEST_CHECK(strcmp(JsonObject_GetStringValue(message, "type"), "device-event-occurred") == 0);

    payload = JsonObject_GetObject(message, "payload");
    TINY_TEST_CHECK(payload != NULL);
    event = (payload != NULL) ? JsonObject_GetObject(payload, "event") : NULL;
    TINY_TEST_CHECK(event != NULL);

    arguments = (event != NULL) ? JsonObject_GetArray(event, "arguments") : NULL;
    TINY_TEST_CHECK(arguments != NULL && JsonArray_GetSize(arguments) == 4);

    first = (arguments != NULL) ? JsonArray_GetObject(arguments, 0) : NULL;
    TINY_TEST_CHECK(first != NULL && JsonNumber_GetInteger(JsonObject_GetNumber(first, "piid")) == 1);

    values = (first != NULL) ? JsonObject_GetArray(first, "values") : NULL;
    TINY_TEST_CHECK(values != NULL && JsonArray_GetSize(values) == 1);
    TINY_TEST_CHECK(values != NULL && JsonArray_GetString(values, 0) != NULL);

    {
        JsonObject *fourth = (arguments != NULL) ? JsonArray_GetObject(arguments, 3) : NULL;
        JsonArray *floats = (fourth != NULL) ? JsonObject_GetArray(fourth, "values") : NULL;

        TINY_TEST_CHECK(floats != NULL && JsonArray_GetSize(floats) == 2);
        TINY_TEST_CHECK(floats != NULL && JsonNumber_GetFloat(JsonArray_GetNumber(floats, 0)) > 10.69);
        TINY_TEST_CHECK(floats != NULL && JsonNumber_GetFloat(JsonArray_GetNumber(floats, 0)) < 10.71);
    }

    JsonObject_Delete(object);
}

int main(void)
{
    TINY_TEST_BEGIN("json-resources");
    test_buffer_resources();
    test_string_resources();
    test_float_resources();
    test_part_resources();
    test_equals_resources();
    test_invalid_resources();
    test_utf8_resource();
    test_owner_event_resource();
    return TINY_TEST_FINISH();
}
