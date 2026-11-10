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

#include <tinycore/buffer.h>
#include <tinycore/json.h>
#include <tinycore/tiny_alloc.h>

static void test_json_string(void)
{
    JsonString *a = JsonString_New("abc");
    JsonString *b = JsonString_NewLength("abcdef", 3);
    JsonString *empty = JsonString_NewLength(NULL, 0);
    JsonString *embedded = JsonString_NewLength("a\0b", 3);
    JsonString *other = JsonString_NewLength("a\0c", 3);

    TINY_TEST_CHECK(JsonString_New(NULL) == NULL);
    TINY_TEST_CHECK(JsonString_NewLength(NULL, 1) == NULL);

    TINY_TEST_CHECK(a != NULL);
    TINY_TEST_CHECK(a != NULL && strcmp(JsonString_GetValue(a), "abc") == 0);
    TINY_TEST_CHECK(a != NULL && JsonString_GetLength(a) == 3);

    TINY_TEST_CHECK(b != NULL);
    TINY_TEST_CHECK(b != NULL && strcmp(JsonString_GetValue(b), "abc") == 0);
    TINY_TEST_CHECK(b != NULL && JsonString_GetLength(b) == 3);

    TINY_TEST_CHECK(empty != NULL);
    TINY_TEST_CHECK(empty != NULL && JsonString_GetLength(empty) == 0);
    TINY_TEST_CHECK(empty != NULL && JsonString_GetValue(empty)[0] == '\0');

    TINY_TEST_CHECK(embedded != NULL && embedded->length == 3);
    TINY_TEST_CHECK(embedded != NULL && memcmp(JsonString_GetValue(embedded), "a\0b", 3) == 0);
    TINY_TEST_CHECK(other != NULL && memcmp(JsonString_GetValue(other), "a\0c", 3) == 0);

    TINY_TEST_CHECK(JsonString_Equals(a, a));
    TINY_TEST_CHECK(a != NULL && b != NULL && JsonString_Equals(a, b));
    TINY_TEST_CHECK(JsonString_Equals(NULL, NULL));
    TINY_TEST_CHECK(!JsonString_Equals(NULL, a));
    TINY_TEST_CHECK(!JsonString_Equals(a, NULL));
    TINY_TEST_CHECK(!JsonString_Equals(embedded, other));
    TINY_TEST_CHECK(empty != NULL && JsonString_Equals(empty, empty));

    TINY_TEST_CHECK(JsonString_GetValue(NULL) == NULL);
    TINY_TEST_CHECK(JsonString_GetLength(NULL) == 0);
    JsonString_Delete(NULL);

    JsonString_Delete(a);
    JsonString_Delete(b);
    JsonString_Delete(empty);
    JsonString_Delete(embedded);
    JsonString_Delete(other);
}

static void test_json_number_and_boolean(void)
{
    JsonNumber *integer = JsonNumber_NewInteger(-5);
    JsonNumber *same = JsonNumber_NewInteger(-5);
    JsonNumber *different = JsonNumber_NewInteger(5);
    JsonNumber *real = JsonNumber_NewFloat(2.75);
    JsonNumber *whole = JsonNumber_NewFloat(3.0);
    JsonNumber *positive = JsonNumber_NewFloat(1.9);
    JsonNumber *negative = JsonNumber_NewFloat(-1.9);
    JsonBoolean *true_value = JsonBoolean_New(true);
    JsonBoolean *false_value = JsonBoolean_New(false);

    TINY_TEST_CHECK(integer != NULL && JsonNumber_IsInteger(integer));
    TINY_TEST_CHECK(integer != NULL && JsonNumber_GetInteger(integer) == -5);
    TINY_TEST_CHECK(integer != NULL && JsonNumber_GetFloat(integer) == -5.0);

    TINY_TEST_CHECK(real != NULL && !JsonNumber_IsInteger(real));
    TINY_TEST_CHECK(real != NULL && JsonNumber_GetFloat(real) == 2.75);
    TINY_TEST_CHECK(real != NULL && JsonNumber_GetInteger(real) == 2);
    TINY_TEST_CHECK(positive != NULL && JsonNumber_GetInteger(positive) == 1);
    TINY_TEST_CHECK(negative != NULL && JsonNumber_GetInteger(negative) == -1);

    TINY_TEST_CHECK(!JsonNumber_IsInteger(NULL));
    TINY_TEST_CHECK(JsonNumber_GetInteger(NULL) == 0);
    TINY_TEST_CHECK(JsonNumber_GetFloat(NULL) == 0.0);

    TINY_TEST_CHECK(JsonNumber_Equals(integer, integer));
    TINY_TEST_CHECK(JsonNumber_Equals(integer, same));
    TINY_TEST_CHECK(!JsonNumber_Equals(integer, different));
    TINY_TEST_CHECK(!JsonNumber_Equals(integer, real));
    TINY_TEST_CHECK(!JsonNumber_Equals(real, whole));
    TINY_TEST_CHECK(JsonNumber_Equals(whole, JsonNumber_NewInteger(3)));
    TINY_TEST_CHECK(JsonNumber_Equals(NULL, NULL));
    TINY_TEST_CHECK(!JsonNumber_Equals(NULL, integer));
    TINY_TEST_CHECK(!JsonNumber_Equals(integer, NULL));

    TINY_TEST_CHECK(true_value != NULL && JsonBoolean_GetValue(true_value));
    TINY_TEST_CHECK(false_value != NULL && !JsonBoolean_GetValue(false_value));
    TINY_TEST_CHECK(!JsonBoolean_GetValue(NULL));
    TINY_TEST_CHECK(JsonBoolean_Equals(true_value, true_value));
    TINY_TEST_CHECK(!JsonBoolean_Equals(true_value, false_value));
    TINY_TEST_CHECK(JsonBoolean_Equals(NULL, NULL));
    TINY_TEST_CHECK(!JsonBoolean_Equals(NULL, true_value));

    JsonNumber_Delete(integer);
    JsonNumber_Delete(same);
    JsonNumber_Delete(different);
    JsonNumber_Delete(real);
    JsonNumber_Delete(whole);
    JsonNumber_Delete(positive);
    JsonNumber_Delete(negative);
    JsonNumber_Delete(NULL);
    JsonBoolean_Delete(true_value);
    JsonBoolean_Delete(false_value);
    JsonBoolean_Delete(NULL);
}

static void test_json_value(void)
{
    JsonValue *undefined = JsonValue_New();
    JsonValue *string = JsonValue_NewString("text");
    JsonValue *string_length = JsonValue_NewStringLength("texttail", 4);
    JsonValue *integer = JsonValue_NewInteger(-7);
    JsonValue *real = JsonValue_NewFloat(0.5);
    JsonValue *boolean = JsonValue_NewBoolean(true);
    JsonValue *null_value = JsonValue_NewNull();
    JsonValue *object = JsonValue_NewObject(JsonObject_New());
    JsonValue *array = JsonValue_NewArray(JsonArray_New());
    JsonValue *generic_string = JsonValue_NewValue(JSON_STRING, JsonString_New("generic"));
    JsonValue *generic_number = JsonValue_NewValue(JSON_NUMBER, JsonNumber_NewInteger(3));
    JsonValue *generic_object = JsonValue_NewValue(JSON_OBJECT, JsonObject_New());
    JsonValue *generic_array = JsonValue_NewValue(JSON_ARRAY, JsonArray_New());
    JsonValue *generic_boolean = JsonValue_NewValue(JSON_BOOLEAN, JsonBoolean_New(false));
    JsonValue *generic_null = JsonValue_NewValue(JSON_NULL, NULL);
    JsonValue *generic_undefined = JsonValue_NewValue(JSON_UNDEFINED, (void *)integer);
    JsonValue *other_null = JsonValue_NewNull();
    JsonValue *other_undefined = JsonValue_New();

    TINY_TEST_CHECK(undefined != NULL && undefined->type == JSON_UNDEFINED);
    TINY_TEST_CHECK(string != NULL && string->type == JSON_STRING);
    TINY_TEST_CHECK(string_length != NULL && string_length->type == JSON_STRING);
    TINY_TEST_CHECK(integer != NULL && integer->type == JSON_NUMBER);
    TINY_TEST_CHECK(real != NULL && real->type == JSON_NUMBER);
    TINY_TEST_CHECK(boolean != NULL && boolean->type == JSON_BOOLEAN);
    TINY_TEST_CHECK(null_value != NULL && null_value->type == JSON_NULL);
    TINY_TEST_CHECK(object != NULL && object->type == JSON_OBJECT);
    TINY_TEST_CHECK(array != NULL && array->type == JSON_ARRAY);

    TINY_TEST_CHECK(JsonValue_NewString(NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewNumber(NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewObject(NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewArray(NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewValue(JSON_STRING, NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewValue(JSON_NUMBER, NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewValue(JSON_OBJECT, NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewValue(JSON_ARRAY, NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewValue(JSON_BOOLEAN, NULL) == NULL);
    TINY_TEST_CHECK(JsonValue_NewValue((JsonValueType)99, NULL) == NULL);

    TINY_TEST_CHECK(generic_string != NULL && generic_string->type == JSON_STRING);
    TINY_TEST_CHECK(generic_string != NULL &&
                    strcmp(JsonString_GetValue(generic_string->data.string), "generic") == 0);
    TINY_TEST_CHECK(generic_number != NULL && generic_number->type == JSON_NUMBER);
    TINY_TEST_CHECK(generic_number != NULL &&
                    JsonNumber_GetInteger(generic_number->data.number) == 3);
    TINY_TEST_CHECK(generic_object != NULL && generic_object->type == JSON_OBJECT);
    TINY_TEST_CHECK(generic_object != NULL && JsonObject_IsEmpty(generic_object->data.object));
    TINY_TEST_CHECK(generic_array != NULL && generic_array->type == JSON_ARRAY);
    TINY_TEST_CHECK(generic_array != NULL && JsonArray_IsEmpty(generic_array->data.array));
    TINY_TEST_CHECK(generic_boolean != NULL && generic_boolean->type == JSON_BOOLEAN);
    TINY_TEST_CHECK(generic_boolean != NULL && !JsonBoolean_GetValue(generic_boolean->data.boolean));
    TINY_TEST_CHECK(generic_null != NULL && generic_null->type == JSON_NULL);
    TINY_TEST_CHECK(generic_undefined != NULL && generic_undefined->type == JSON_UNDEFINED);

    TINY_TEST_CHECK(JsonValue_Equals(undefined, other_undefined));
    TINY_TEST_CHECK(JsonValue_Equals(null_value, other_null));
    TINY_TEST_CHECK(!JsonValue_Equals(string, integer));
    TINY_TEST_CHECK(!JsonValue_Equals(string_length, integer));
    TINY_TEST_CHECK(JsonValue_Equals(NULL, NULL));
    TINY_TEST_CHECK(!JsonValue_Equals(NULL, undefined));
    TINY_TEST_CHECK(!JsonValue_Equals(undefined, NULL));

    {
        JsonValue *copy = JsonValue_NewFrom(string);

        TINY_TEST_CHECK(copy != NULL && copy != string);
        TINY_TEST_CHECK(copy != NULL && copy->data.string != string->data.string);
        TINY_TEST_CHECK(JsonValue_Equals(copy, string));
        JsonValue_Delete(copy);
    }

    {
        JsonValue *copy = JsonValue_NewFrom(object);

        TINY_TEST_CHECK(copy != NULL && copy->data.object != object->data.object);
        TINY_TEST_CHECK(JsonValue_Equals(copy, object));
        JsonValue_Delete(copy);
    }

    {
        JsonValue *copy = JsonValue_NewFrom(array);

        TINY_TEST_CHECK(copy != NULL && copy->data.array != array->data.array);
        TINY_TEST_CHECK(JsonValue_Equals(copy, array));
        JsonValue_Delete(copy);
    }

    {
        JsonValue *copy = JsonValue_NewFrom(real);

        TINY_TEST_CHECK(copy != NULL && JsonValue_Equals(copy, real));
        JsonValue_Delete(copy);
    }

    TINY_TEST_CHECK(JsonValue_NewFrom(NULL) == NULL);

    JsonValue_Delete(undefined);
    JsonValue_Delete(string);
    JsonValue_Delete(string_length);
    JsonValue_Delete(integer);
    JsonValue_Delete(real);
    JsonValue_Delete(boolean);
    JsonValue_Delete(null_value);
    JsonValue_Delete(object);
    JsonValue_Delete(array);
    JsonValue_Delete(generic_string);
    JsonValue_Delete(generic_number);
    JsonValue_Delete(generic_object);
    JsonValue_Delete(generic_array);
    JsonValue_Delete(generic_boolean);
    JsonValue_Delete(generic_null);
    JsonValue_Delete(generic_undefined);
    JsonValue_Delete(other_null);
    JsonValue_Delete(other_undefined);
    JsonValue_Delete(NULL);

    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_UNDEFINED), "undefined") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_STRING), "string") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_NUMBER), "number") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_OBJECT), "object") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_ARRAY), "array") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_BOOLEAN), "boolean") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString(JSON_NULL), "null") == 0);
    TINY_TEST_CHECK(strcmp(JsonValueType_ToString((JsonValueType)99), "unknown") == 0);
}

static void test_json_array(void)
{
    JsonArray *array = JsonArray_New();
    JsonArray *copy = NULL;
    JsonValue *undefined = JsonValue_New();
    JsonValue *orphan = JsonValue_NewInteger(1);

    TINY_TEST_CHECK(array != NULL);
    TINY_TEST_CHECK(JsonArray_IsEmpty(array));
    TINY_TEST_CHECK(JsonArray_GetSize(array) == 0);
    TINY_TEST_CHECK(JsonArray_CheckValuesType(array, JSON_NUMBER));

    TINY_TEST_CHECK(JsonArray_Construct(NULL) == TINY_RET_E_NULL_PTR);
    JsonArray_Dispose(NULL);
    JsonArray_Delete(NULL);
    TINY_TEST_CHECK(JsonArray_NewFrom(NULL) == NULL);

    TINY_TEST_CHECK(JsonArray_AddValue(array, undefined) == TINY_RET_E_PARAM_INVALID);
    JsonValue_Delete(undefined);
    TINY_TEST_CHECK(JsonArray_AddValue(array, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonArray_AddValue(NULL, orphan) == TINY_RET_E_NULL_PTR);
    JsonValue_Delete(orphan);
    TINY_TEST_CHECK(JsonArray_AddString(array, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonArray_AddNumber(array, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonArray_AddObject(array, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonArray_AddArray(array, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonArray_AddInteger(NULL, 1) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonArray_AddNull(NULL) == TINY_RET_E_NULL_PTR);

    TINY_TEST_CHECK(JsonArray_AddString(array, "s") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddInteger(array, 7) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddFloat(array, 0.25) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddBoolean(array, true) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddNull(array) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddNumber(array, JsonNumber_NewInteger(9)) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddObject(array, JsonObject_New()) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddArray(array, JsonArray_New()) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonArray_GetSize(array) == 8);
    TINY_TEST_CHECK(!JsonArray_IsEmpty(array));
    TINY_TEST_CHECK(JsonArray_GetValue(array, 8) == NULL);
    TINY_TEST_CHECK(JsonArray_GetValue(NULL, 0) == NULL);

    TINY_TEST_CHECK(JsonArray_GetString(array, 0) != NULL &&
                    strcmp(JsonString_GetValue(JsonArray_GetString(array, 0)), "s") == 0);
    TINY_TEST_CHECK(JsonArray_GetString(array, 1) == NULL);
    TINY_TEST_CHECK(JsonArray_GetNumber(array, 1) != NULL &&
                    JsonNumber_GetInteger(JsonArray_GetNumber(array, 1)) == 7);
    TINY_TEST_CHECK(JsonArray_GetNumber(array, 0) == NULL);
    TINY_TEST_CHECK(JsonArray_GetNumber(array, 2) != NULL &&
                    JsonNumber_GetFloat(JsonArray_GetNumber(array, 2)) == 0.25);
    TINY_TEST_CHECK(JsonArray_GetBoolean(array, 3) != NULL &&
                    JsonBoolean_GetValue(JsonArray_GetBoolean(array, 3)));
    TINY_TEST_CHECK(JsonArray_GetBoolean(array, 4) == NULL);
    TINY_TEST_CHECK(JsonArray_GetObject(array, 6) != NULL);
    TINY_TEST_CHECK(JsonArray_GetObject(array, 7) == NULL);
    TINY_TEST_CHECK(JsonArray_GetArray(array, 6) == NULL);
    TINY_TEST_CHECK(JsonArray_GetArray(array, 7) != NULL);
    TINY_TEST_CHECK(JsonArray_GetValue(array, 5) != NULL &&
                    JsonArray_GetValue(array, 5)->type == JSON_NUMBER);

    TINY_TEST_CHECK(!JsonArray_CheckValuesType(array, JSON_STRING));
    TINY_TEST_CHECK(!JsonArray_CheckValuesType(array, JSON_NUMBER));
    TINY_TEST_CHECK(!JsonArray_CheckValuesType(NULL, JSON_STRING));

    TINY_TEST_CHECK(JsonArray_Equals(array, array));
    TINY_TEST_CHECK(JsonArray_Equals(NULL, NULL));
    TINY_TEST_CHECK(!JsonArray_Equals(array, NULL));
    TINY_TEST_CHECK(!JsonArray_Equals(NULL, array));

    copy = JsonArray_NewFrom(array);
    TINY_TEST_CHECK(copy != NULL && copy != array);
    TINY_TEST_CHECK(JsonArray_Equals(array, copy));
    TINY_TEST_CHECK(JsonArray_GetValue(copy, 0) != JsonArray_GetValue(array, 0));

    TINY_TEST_CHECK(JsonArray_AddInteger(array, 1) == TINY_RET_OK);
    TINY_TEST_CHECK(!JsonArray_Equals(array, copy));

    JsonArray_Clean(copy);
    TINY_TEST_CHECK(JsonArray_GetSize(copy) == 0);
    TINY_TEST_CHECK(JsonArray_IsEmpty(copy));
    JsonArray_Clean(NULL);

    {
        JsonArray stack_array;

        TINY_TEST_CHECK(JsonArray_Construct(&stack_array) == TINY_RET_OK);
        TINY_TEST_CHECK(JsonArray_AddInteger(&stack_array, 1) == TINY_RET_OK);
        TINY_TEST_CHECK(JsonArray_GetSize(&stack_array) == 1);
        JsonArray_Dispose(&stack_array);
    }

    JsonArray_Delete(array);
    JsonArray_Delete(copy);
}

static void test_json_object(void)
{
    JsonObject *object = JsonObject_New();
    JsonObject *copy = NULL;
    JsonValue *source = NULL;
    JsonValue *orphan = NULL;
    Buffer buffer;
    char *compact = NULL;
    char *pretty = NULL;
    size_t compact_length = 0;

    TINY_TEST_CHECK(object != NULL);
    TINY_TEST_CHECK(JsonObject_IsEmpty(object));
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 0);

    TINY_TEST_CHECK(JsonObject_Construct(NULL) == TINY_RET_E_NULL_PTR);
    JsonObject_Dispose(NULL);
    JsonObject_Delete(NULL);
    TINY_TEST_CHECK(JsonObject_NewFrom(NULL) == NULL);
    TINY_TEST_CHECK(JsonObject_NewString(NULL) == NULL);
    TINY_TEST_CHECK(JsonObject_NewStringFast(NULL) == NULL);

    TINY_TEST_CHECK(JsonObject_PutString(object, "s", "value") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutInteger(object, "i", 5) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutFloat(object, "f", 1.5) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutBoolean(object, "b", true) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutNull(object, "n") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutNumber(object, "num", JsonNumber_NewInteger(6)) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutObject(object, "o", JsonObject_New()) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_PutArray(object, "a", JsonArray_New()) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonObject_GetSize(object) == 8);
    TINY_TEST_CHECK(!JsonObject_IsEmpty(object));

    TINY_TEST_CHECK(JsonObject_ContainsKey(object, "s"));
    TINY_TEST_CHECK(!JsonObject_ContainsKey(object, "missing"));
    TINY_TEST_CHECK(!JsonObject_ContainsKey(object, NULL));
    TINY_TEST_CHECK(!JsonObject_ContainsKey(NULL, "s"));

    TINY_TEST_CHECK(strcmp(JsonObject_GetStringValue(object, "s"), "value") == 0);
    TINY_TEST_CHECK(JsonObject_GetStringValue(object, "missing") == NULL);
    TINY_TEST_CHECK(JsonObject_GetStringValue(object, "i") == NULL);

    TINY_TEST_CHECK(JsonObject_GetString(object, "s") != NULL);
    TINY_TEST_CHECK(JsonObject_GetString(object, "i") == NULL);
    TINY_TEST_CHECK(JsonObject_GetNumber(object, "i") != NULL &&
                    JsonNumber_GetInteger(JsonObject_GetNumber(object, "i")) == 5);
    TINY_TEST_CHECK(JsonObject_GetNumber(object, "f") != NULL &&
                    JsonNumber_GetFloat(JsonObject_GetNumber(object, "f")) == 1.5);
    TINY_TEST_CHECK(JsonObject_GetNumber(object, "s") == NULL);
    TINY_TEST_CHECK(JsonObject_GetNumber(object, "num") != NULL &&
                    JsonNumber_GetInteger(JsonObject_GetNumber(object, "num")) == 6);
    TINY_TEST_CHECK(JsonObject_GetBoolean(object, "b") != NULL &&
                    JsonBoolean_GetValue(JsonObject_GetBoolean(object, "b")));
    TINY_TEST_CHECK(JsonObject_GetBoolean(object, "n") == NULL);
    TINY_TEST_CHECK(JsonObject_GetObject(object, "o") != NULL);
    TINY_TEST_CHECK(JsonObject_GetObject(object, "a") == NULL);
    TINY_TEST_CHECK(JsonObject_GetArray(object, "a") != NULL);
    TINY_TEST_CHECK(JsonObject_GetArray(object, "o") == NULL);
    TINY_TEST_CHECK(JsonObject_GetValue(object, "missing") == NULL);
    TINY_TEST_CHECK(JsonObject_GetValue(object, NULL) == NULL);
    TINY_TEST_CHECK(JsonObject_GetValue(NULL, "s") == NULL);
    TINY_TEST_CHECK(JsonObject_GetValue(object, "n") != NULL &&
                    JsonObject_GetValue(object, "n")->type == JSON_NULL);

    TINY_TEST_CHECK(JsonObject_PutInteger(object, "i", 6) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 8);
    TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(object, "i")) == 6);

    TINY_TEST_CHECK(JsonObject_PutInteger(object, "", 42) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 9);
    TINY_TEST_CHECK(JsonObject_ContainsKey(object, ""));
    TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(object, "")) == 42);

    orphan = JsonValue_NewInteger(1);
    TINY_TEST_CHECK(JsonObject_PutValue(object, NULL, orphan) == TINY_RET_E_NULL_PTR);
    JsonValue_Delete(orphan);
    orphan = JsonValue_NewInteger(1);
    TINY_TEST_CHECK(JsonObject_PutValue(object, "x", orphan) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_Remove(object, "x") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 9);

    orphan = JsonValue_NewInteger(1);
    TINY_TEST_CHECK(JsonObject_PutValue(NULL, "k", orphan) == TINY_RET_E_NULL_PTR);
    JsonValue_Delete(orphan);
    TINY_TEST_CHECK(JsonObject_PutString(object, "s", NULL) != TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_AddValue(object, "missing_value", NULL) != TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 9);

    source = JsonValue_NewInteger(11);
    TINY_TEST_CHECK(JsonObject_AddValue(object, "copy", source) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 10);
    TINY_TEST_CHECK(JsonObject_GetValue(object, "copy") != source);
    TINY_TEST_CHECK(JsonValue_Equals(JsonObject_GetValue(object, "copy"), source));
    JsonValue_Delete(source);
    TINY_TEST_CHECK(JsonNumber_GetInteger(JsonObject_GetNumber(object, "copy")) == 11);

    copy = JsonObject_NewFrom(object);
    TINY_TEST_CHECK(copy != NULL && copy != object);
    TINY_TEST_CHECK(JsonObject_Equals(object, copy));
    TINY_TEST_CHECK(JsonObject_Equals(object, object));
    TINY_TEST_CHECK(JsonObject_Equals(NULL, NULL));
    TINY_TEST_CHECK(!JsonObject_Equals(object, NULL));
    TINY_TEST_CHECK(!JsonObject_Equals(NULL, object));

    TINY_TEST_CHECK(JsonObject_Remove(object, "") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_GetSize(object) == 9);
    TINY_TEST_CHECK(!JsonObject_Equals(object, copy));
    TINY_TEST_CHECK(JsonObject_Remove(object, "missing") == TINY_RET_E_NOT_FOUND);
    TINY_TEST_CHECK(JsonObject_Remove(NULL, "s") == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonObject_Remove(object, NULL) != TINY_RET_OK);

    JsonObject_Delete(copy);

    {
        JsonObject stack_object;

        TINY_TEST_CHECK(JsonObject_Construct(&stack_object) == TINY_RET_OK);
        TINY_TEST_CHECK(JsonObject_PutInteger(&stack_object, "k", 1) == TINY_RET_OK);
        TINY_TEST_CHECK(JsonObject_GetSize(&stack_object) == 1);
        JsonObject_Dispose(&stack_object);
    }

    compact = JsonObject_ToString(object, false);
    TINY_TEST_CHECK(compact != NULL);
    TINY_TEST_CHECK(JsonObject_ToString(NULL, false) == NULL);
    TINY_TEST_CHECK(JsonEncoder_ToString(NULL, false) == NULL);
    if (compact == NULL)
    {
        JsonObject_Delete(object);
        return;
    }
    compact_length = strlen(compact);

    TINY_TEST_CHECK(Buffer_Construct(&buffer, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonObject_Encode(object, false, &buffer) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == compact_length);
    TINY_TEST_CHECK(memcmp(Buffer_GetBytesConst(&buffer), compact, compact_length) == 0);

    pretty = JsonObject_ToString(object, true);
    TINY_TEST_CHECK(pretty != NULL && strlen(pretty) > compact_length);
    TINY_TEST_CHECK(JsonObject_Encode(object, true, &buffer) == TINY_RET_OK);
    TINY_TEST_CHECK(Buffer_GetLength(&buffer) == strlen(pretty));
    TINY_TEST_CHECK(memcmp(Buffer_GetBytesConst(&buffer), pretty, strlen(pretty)) == 0);

    TINY_TEST_CHECK(JsonObject_Encode(NULL, false, &buffer) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonObject_Encode(object, false, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonEncoder_EncodeToBuffer(object, false, NULL) == TINY_RET_E_NULL_PTR);

    Buffer_Dispose(&buffer);
    tiny_free(compact);
    tiny_free(pretty);
    JsonObject_Delete(object);
}

typedef struct OutputCollector
{
    Buffer buffer;
    int calls;
    int fail_at;
} OutputCollector;

static TINYCORE_RET collect_output(const char *data, size_t length, void *context)
{
    OutputCollector *collector = (OutputCollector *)context;

    ++collector->calls;
    if (collector->fail_at > 0 && collector->calls >= collector->fail_at)
    {
        return TINY_RET_E_IO;
    }

    return Buffer_Append(&collector->buffer, data, length);
}

static void test_json_encoder(void)
{
    JsonObject *object = JsonObject_NewString("{\"a\":1,\"b\":[true,null],\"c\":\"x\"}");
    JsonObject *empty = NULL;
    OutputCollector collector;
    OutputCollector failing;
    char *expected = NULL;
    size_t expected_length = 0;

    TINY_TEST_CHECK(object != NULL);
    if (object == NULL)
    {
        return;
    }

    expected = JsonObject_ToString(object, false);
    TINY_TEST_CHECK(expected != NULL);
    if (expected == NULL)
    {
        JsonObject_Delete(object);
        return;
    }
    expected_length = strlen(expected);

    memset(&collector, 0, sizeof(collector));
    TINY_TEST_CHECK(Buffer_Construct(&collector.buffer, 0) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonEncoder_Encode(object, false, collect_output, &collector) == TINY_RET_OK);
    TINY_TEST_CHECK(collector.calls > 0);
    TINY_TEST_CHECK(Buffer_GetLength(&collector.buffer) == expected_length);
    TINY_TEST_CHECK(memcmp(Buffer_GetBytesConst(&collector.buffer),
                           expected,
                           expected_length) == 0);

    memset(&failing, 0, sizeof(failing));
    TINY_TEST_CHECK(Buffer_Construct(&failing.buffer, 0) == TINY_RET_OK);
    failing.fail_at = 1;
    TINY_TEST_CHECK(JsonEncoder_Encode(object, false, collect_output, &failing) == TINY_RET_E_IO);
    TINY_TEST_CHECK(failing.calls == 1);

    TINY_TEST_CHECK(JsonEncoder_Encode(object, false, NULL, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonEncoder_Encode(NULL, false, collect_output, &collector) ==
                    TINY_RET_E_NULL_PTR);

    empty = JsonObject_New();
    TINY_TEST_CHECK(empty != NULL);
    TINY_TEST_CHECK(JsonEncoder_Encode(empty, false, collect_output, &collector) == TINY_RET_OK);

    Buffer_Dispose(&collector.buffer);
    Buffer_Dispose(&failing.buffer);
    JsonObject_Delete(empty);
    JsonObject_Delete(object);
    tiny_free(expected);
}

static void test_json_token(void)
{
    JsonToken *token = JsonToken_New(JSON_TOKEN_STRING, 4, 9);

    TINY_TEST_CHECK(token != NULL);
    if (token != NULL)
    {
        TINY_TEST_CHECK(token->type == JSON_TOKEN_STRING);
        TINY_TEST_CHECK(token->offset == 4);
        TINY_TEST_CHECK(token->length == 9);
        JsonToken_Delete(token);
    }
    JsonToken_Delete(NULL);

    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_OBJECT_START), "{") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_OBJECT_END), "}") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_ARRAY_START), "[") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_ARRAY_END), "]") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_COMMA), ",") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_COLON), ":") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_NULL), "null") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_TRUE), "true") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_FALSE), "false") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_STRING), "string") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_NUMBER), "number") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString(JSON_TOKEN_INVALID), "invalid") == 0);
    TINY_TEST_CHECK(strcmp(JsonTokenType_ToString((JsonTokenType)42), "invalid") == 0);
}

static void expect_tokenizer_error(JsonTokenizer *tokenizer, const char *text)
{
    TINY_TEST_CHECK_MSG(JsonTokenizer_Parse(tokenizer, text, strlen(text)) ==
                            TINY_RET_E_INVALID_DATA,
                        "tokenizer accepted invalid document: %s",
                        text);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(tokenizer) == 0);
}

static void test_json_tokenizer(void)
{
    JsonTokenizer tokenizer;
    const char *document = "{\"a\":1}";
    const char *array_document = " \t[1, true, false, null]\n";
    JsonToken *head = NULL;

    TINY_TEST_CHECK(JsonTokenizer_Construct(NULL) == TINY_RET_E_NULL_PTR);
    JsonTokenizer_Dispose(NULL);
    TINY_TEST_CHECK(JsonTokenizer_Construct(&tokenizer) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonTokenizer_Parse(NULL, document, strlen(document)) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer, NULL, 0) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 0);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(NULL) == 0);
    TINY_TEST_CHECK(JsonTokenizer_Head(NULL) == NULL);
    JsonTokenizer_Pop(NULL);

    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer, document, strlen(document)) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 5);

    head = JsonTokenizer_Head(&tokenizer);
    TINY_TEST_CHECK(head != NULL && head->type == JSON_TOKEN_OBJECT_START &&
                    head->offset == 0 && head->length == 1);
    JsonTokenizer_Pop(&tokenizer);

    head = JsonTokenizer_Head(&tokenizer);
    TINY_TEST_CHECK(head != NULL && head->type == JSON_TOKEN_STRING &&
                    head->offset == 1 && head->length == 3);
    JsonTokenizer_Pop(&tokenizer);

    head = JsonTokenizer_Head(&tokenizer);
    TINY_TEST_CHECK(head != NULL && head->type == JSON_TOKEN_COLON &&
                    head->offset == 4 && head->length == 1);
    JsonTokenizer_Pop(&tokenizer);

    head = JsonTokenizer_Head(&tokenizer);
    TINY_TEST_CHECK(head != NULL && head->type == JSON_TOKEN_NUMBER &&
                    head->offset == 5 && head->length == 1);
    JsonTokenizer_Pop(&tokenizer);

    head = JsonTokenizer_Head(&tokenizer);
    TINY_TEST_CHECK(head != NULL && head->type == JSON_TOKEN_OBJECT_END &&
                    head->offset == 6 && head->length == 1);
    JsonTokenizer_Pop(&tokenizer);

    TINY_TEST_CHECK(JsonTokenizer_Head(&tokenizer) == NULL);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 0);

    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer,
                                        array_document,
                                        strlen(array_document)) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 9);
    {
        static const JsonTokenType expected[] = {
            JSON_TOKEN_ARRAY_START,
            JSON_TOKEN_NUMBER,
            JSON_TOKEN_COMMA,
            JSON_TOKEN_TRUE,
            JSON_TOKEN_COMMA,
            JSON_TOKEN_FALSE,
            JSON_TOKEN_COMMA,
            JSON_TOKEN_NULL,
            JSON_TOKEN_ARRAY_END
        };
        size_t i = 0;

        for (i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i)
        {
            head = JsonTokenizer_Head(&tokenizer);
            TINY_TEST_CHECK(head != NULL && head->type == expected[i]);
            JsonTokenizer_Pop(&tokenizer);
        }
    }

    /* Parsing again discards the previous token queue. */
    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer, "{\"b\":2}", 7) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 5);

    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer, "   ", 3) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 0);

    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer, "\"\\u4e2d\"", 8) == TINY_RET_OK);
    head = JsonTokenizer_Head(&tokenizer);
    TINY_TEST_CHECK(head != NULL && head->type == JSON_TOKEN_STRING &&
                    head->offset == 0 && head->length == 8);

    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer, "\"\xE4\xB8\xAD\"", 5) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 1);

    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer, "\"\xF0\x9F\x98\x80\"", 6) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 1);

    TINY_TEST_CHECK(JsonTokenizer_Parse(&tokenizer, "true", 4) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonTokenizer_GetSize(&tokenizer) == 1);

    expect_tokenizer_error(&tokenizer, "\"\\x\"");
    expect_tokenizer_error(&tokenizer, "\"\\u12\"");
    expect_tokenizer_error(&tokenizer, "\"\\u12G4\"");
    expect_tokenizer_error(&tokenizer, "\"a\nb\"");
    expect_tokenizer_error(&tokenizer, "\"unterminated");
    expect_tokenizer_error(&tokenizer, "1x");
    expect_tokenizer_error(&tokenizer, "01");
    expect_tokenizer_error(&tokenizer, "-");
    expect_tokenizer_error(&tokenizer, "1.");
    expect_tokenizer_error(&tokenizer, "1e");
    expect_tokenizer_error(&tokenizer, "truex");
    expect_tokenizer_error(&tokenizer, "nul");
    expect_tokenizer_error(&tokenizer, "\"\xC0\x80\"");
    expect_tokenizer_error(&tokenizer, "\"\xE0\x80\x80\"");
    expect_tokenizer_error(&tokenizer, "\"\xED\xA0\x80\"");
    expect_tokenizer_error(&tokenizer, "@");

    JsonTokenizer_Dispose(&tokenizer);
}

static void test_json_decoder(void)
{
    JsonDecoder *heap = JsonDecoder_New();
    JsonDecoder decoder;
    JsonDecoderStatus status = JSON_DECODER_STATUS_OK;
    JsonObject *object = NULL;
    CycleBuffer cycle;

    TINY_TEST_CHECK(heap != NULL);
    TINY_TEST_CHECK(JsonDecoder_Construct(NULL) == TINY_RET_E_NULL_PTR);
    JsonDecoder_Dispose(NULL);
    JsonDecoder_Delete(NULL);

    if (heap != NULL)
    {
        object = JsonDecoder_Parse(heap, "{\"heap\":true}", &status);
        TINY_TEST_CHECK(status == JSON_DECODER_STATUS_OK);
        TINY_TEST_CHECK(object != NULL &&
                        JsonBoolean_GetValue(JsonObject_GetBoolean(object, "heap")));
        JsonObject_Delete(object);
        JsonDecoder_Delete(heap);
    }

    TINY_TEST_CHECK(JsonDecoder_Construct(&decoder) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonDecoder_Parse(NULL, "{}", &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(JsonDecoder_Parse(&decoder, NULL, &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);

    object = JsonDecoder_Parse(&decoder, "{\"a\":1}", NULL);
    TINY_TEST_CHECK(object != NULL &&
                    JsonNumber_GetInteger(JsonObject_GetNumber(object, "a")) == 1);
    JsonObject_Delete(object);

    TINY_TEST_CHECK(JsonDecoder_Parse(&decoder, "", &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INCOMPLETE);
    TINY_TEST_CHECK(JsonDecoder_Parse(&decoder, "{\"a\":1", &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INCOMPLETE);
    TINY_TEST_CHECK(JsonDecoder_Parse(&decoder, "{\"a\":1}x", &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(JsonDecoder_Parse(&decoder, "[1]", &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(JsonDecoder_Parse(&decoder, "{\"a\":}", &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);

    object = JsonDecoder_Parse(&decoder, "  {\"a\":1}  ", &status);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_OK && object != NULL);
    JsonObject_Delete(object);

    /* Start from a clean buffer for the incremental API. */
    JsonDecoder_Dispose(&decoder);
    TINY_TEST_CHECK(JsonDecoder_Construct(&decoder) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonDecoder_Feed(NULL, "{}", 2) == TINY_RET_E_NULL_PTR);

    object = JsonDecoder_ParseBuffered(NULL, &status);
    TINY_TEST_CHECK(object == NULL && status == JSON_DECODER_STATUS_INVALID);

    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(object == NULL && status == JSON_DECODER_STATUS_INCOMPLETE);

    TINY_TEST_CHECK(JsonDecoder_Feed(&decoder, "x", 1) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonDecoder_ParseBuffered(&decoder, &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);

    TINY_TEST_CHECK(JsonDecoder_Feed(&decoder, "{\"ok\":true}", 11) == TINY_RET_OK);
    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_OK && object != NULL);
    TINY_TEST_CHECK(object != NULL &&
                    JsonBoolean_GetValue(JsonObject_GetBoolean(object, "ok")));
    JsonObject_Delete(object);

    TINY_TEST_CHECK(JsonDecoder_ParseBuffered(&decoder, &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INCOMPLETE);

    /* Two documents fed together are decoded one at a time. */
    TINY_TEST_CHECK(JsonDecoder_Feed(&decoder, "{\"a\":1}{\"b\":2}", 14) == TINY_RET_OK);
    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_OK && object != NULL);
    TINY_TEST_CHECK(object != NULL &&
                    JsonNumber_GetInteger(JsonObject_GetNumber(object, "a")) == 1);
    JsonObject_Delete(object);

    object = JsonDecoder_ParseBuffered(&decoder, &status);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_OK && object != NULL);
    TINY_TEST_CHECK(object != NULL &&
                    JsonNumber_GetInteger(JsonObject_GetNumber(object, "b")) == 2);
    JsonObject_Delete(object);

    TINY_TEST_CHECK(JsonDecoder_ParseBuffered(&decoder, &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INCOMPLETE);

    TINY_TEST_CHECK(CycleBuffer_Construct(&cycle, 64) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonDecoder_ParseBuffer(NULL, &cycle, &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);
    TINY_TEST_CHECK(JsonDecoder_ParseBuffer(&decoder, NULL, &status) == NULL);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_INVALID);

    TINY_TEST_CHECK(CycleBuffer_Write(&cycle, "{\"c\":3}", 7) == 7);
    object = JsonDecoder_ParseBuffer(&decoder, &cycle, &status);
    TINY_TEST_CHECK(status == JSON_DECODER_STATUS_OK && object != NULL);
    TINY_TEST_CHECK(object != NULL &&
                    JsonNumber_GetInteger(JsonObject_GetNumber(object, "c")) == 3);
    JsonObject_Delete(object);
    TINY_TEST_CHECK(CycleBuffer_IsEmpty(&cycle));

    CycleBuffer_Dispose(&cycle);
    JsonDecoder_Dispose(&decoder);
}

int main(void)
{
    TINY_TEST_BEGIN("json_model");
    test_json_string();
    test_json_number_and_boolean();
    test_json_value();
    test_json_array();
    test_json_object();
    test_json_encoder();
    test_json_token();
    test_json_tokenizer();
    test_json_decoder();
    return TINY_TEST_FINISH();
}


