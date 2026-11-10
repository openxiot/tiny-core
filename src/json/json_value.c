/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_array.h>
#include <tinycore/json_boolean.h>
#include <tinycore/json_number.h>
#include <tinycore/json_object.h>
#include <tinycore/json_string.h>
#include <tinycore/json_value.h>
#include <tinycore/tiny_alloc.h>

static JsonValue *json_value_alloc(JsonValueType type)
{
    JsonValue *value = (JsonValue *)tiny_malloc(sizeof(JsonValue));

    if (value == NULL)
    {
        return NULL;
    }

    value->type = type;
    memset(&value->data, 0, sizeof(value->data));

    return value;
}

JsonValue *JsonValue_New(void)
{
    return json_value_alloc(JSON_UNDEFINED);
}

JsonValue *JsonValue_NewStringLength(const char *string, size_t length)
{
    JsonValue *value = json_value_alloc(JSON_STRING);
    JsonString *data = NULL;

    if (value == NULL)
    {
        return NULL;
    }

    data = JsonString_NewLength(string, length);
    if (data == NULL)
    {
        tiny_free(value);
        return NULL;
    }

    value->data.string = data;
    return value;
}

JsonValue *JsonValue_NewString(const char *string)
{
    if (string == NULL)
    {
        return NULL;
    }

    return JsonValue_NewStringLength(string, strlen(string));
}

JsonValue *JsonValue_NewNumber(JsonNumber *number)
{
    JsonValue *value = NULL;

    if (number == NULL)
    {
        return NULL;
    }

    value = json_value_alloc(JSON_NUMBER);
    if (value == NULL)
    {
        JsonNumber_Delete(number);
        return NULL;
    }

    value->data.number = number;
    return value;
}

JsonValue *JsonValue_NewInteger(int32_t integer)
{
    return JsonValue_NewNumber(JsonNumber_NewInteger(integer));
}

JsonValue *JsonValue_NewFloat(double real)
{
    return JsonValue_NewNumber(JsonNumber_NewFloat(real));
}

JsonValue *JsonValue_NewBoolean(bool boolean)
{
    JsonValue *value = json_value_alloc(JSON_BOOLEAN);
    JsonBoolean *data = NULL;

    if (value == NULL)
    {
        return NULL;
    }

    data = JsonBoolean_New(boolean);
    if (data == NULL)
    {
        tiny_free(value);
        return NULL;
    }

    value->data.boolean = data;
    return value;
}

JsonValue *JsonValue_NewNull(void)
{
    return json_value_alloc(JSON_NULL);
}

JsonValue *JsonValue_NewObject(JsonObject *object)
{
    JsonValue *value = NULL;

    if (object == NULL)
    {
        return NULL;
    }

    value = json_value_alloc(JSON_OBJECT);
    if (value == NULL)
    {
        JsonObject_Delete(object);
        return NULL;
    }

    value->data.object = object;
    return value;
}

JsonValue *JsonValue_NewArray(JsonArray *array)
{
    JsonValue *value = NULL;

    if (array == NULL)
    {
        return NULL;
    }

    value = json_value_alloc(JSON_ARRAY);
    if (value == NULL)
    {
        JsonArray_Delete(array);
        return NULL;
    }

    value->data.array = array;
    return value;
}

JsonValue *JsonValue_NewValue(JsonValueType type, void *data)
{
    JsonValue *value = NULL;

    switch (type)
    {
        case JSON_UNDEFINED:
            return JsonValue_New();

        case JSON_NULL:
            return JsonValue_NewNull();

        case JSON_STRING:
            if (data == NULL)
            {
                return NULL;
            }
            value = json_value_alloc(JSON_STRING);
            if (value != NULL)
            {
                value->data.string = (JsonString *)data;
            }
            return value;

        case JSON_NUMBER:
            if (data == NULL)
            {
                return NULL;
            }
            value = json_value_alloc(JSON_NUMBER);
            if (value != NULL)
            {
                value->data.number = (JsonNumber *)data;
            }
            return value;

        case JSON_OBJECT:
            if (data == NULL)
            {
                return NULL;
            }
            value = json_value_alloc(JSON_OBJECT);
            if (value != NULL)
            {
                value->data.object = (JsonObject *)data;
            }
            return value;

        case JSON_ARRAY:
            if (data == NULL)
            {
                return NULL;
            }
            value = json_value_alloc(JSON_ARRAY);
            if (value != NULL)
            {
                value->data.array = (JsonArray *)data;
            }
            return value;

        case JSON_BOOLEAN:
            if (data == NULL)
            {
                return NULL;
            }
            value = json_value_alloc(JSON_BOOLEAN);
            if (value != NULL)
            {
                value->data.boolean = (JsonBoolean *)data;
            }
            return value;

        default:
            return NULL;
    }
}

JsonValue *JsonValue_NewFrom(const JsonValue *other)
{
    if (other == NULL)
    {
        return NULL;
    }

    switch (other->type)
    {
        case JSON_UNDEFINED:
            return JsonValue_New();

        case JSON_NULL:
            return JsonValue_NewNull();

        case JSON_STRING:
            if (other->data.string == NULL)
            {
                return NULL;
            }
            return JsonValue_NewStringLength(other->data.string->value,
                                             other->data.string->length);

        case JSON_NUMBER:
            if (other->data.number == NULL)
            {
                return NULL;
            }
            if (other->data.number->type == JSON_NUMBER_INTEGER)
            {
                return JsonValue_NewInteger(other->data.number->value.intValue);
            }
            return JsonValue_NewFloat(other->data.number->value.floatValue);

        case JSON_OBJECT:
            if (other->data.object == NULL)
            {
                return NULL;
            }
            return JsonValue_NewObject(JsonObject_NewFrom(other->data.object));

        case JSON_ARRAY:
            if (other->data.array == NULL)
            {
                return NULL;
            }
            return JsonValue_NewArray(JsonArray_NewFrom(other->data.array));

        case JSON_BOOLEAN:
            if (other->data.boolean == NULL)
            {
                return NULL;
            }
            return JsonValue_NewBoolean(other->data.boolean->value);

        default:
            return NULL;
    }
}

void JsonValue_Delete(JsonValue *value)
{
    if (value == NULL)
    {
        return;
    }

    switch (value->type)
    {
        case JSON_STRING:
            JsonString_Delete(value->data.string);
            break;

        case JSON_NUMBER:
            JsonNumber_Delete(value->data.number);
            break;

        case JSON_OBJECT:
            JsonObject_Delete(value->data.object);
            break;

        case JSON_ARRAY:
            JsonArray_Delete(value->data.array);
            break;

        case JSON_BOOLEAN:
            JsonBoolean_Delete(value->data.boolean);
            break;

        case JSON_UNDEFINED:
        case JSON_NULL:
        default:
            break;
    }

    value->type = JSON_UNDEFINED;
    memset(&value->data, 0, sizeof(value->data));
    tiny_free(value);
}

bool JsonValue_Equals(const JsonValue *value, const JsonValue *other)
{
    if (value == other)
    {
        return true;
    }

    if (value == NULL || other == NULL)
    {
        return false;
    }

    if (value->type != other->type)
    {
        return false;
    }

    switch (value->type)
    {
        case JSON_STRING:
            return JsonString_Equals(value->data.string, other->data.string);

        case JSON_NUMBER:
            return JsonNumber_Equals(value->data.number, other->data.number);

        case JSON_OBJECT:
            return JsonObject_Equals(value->data.object, other->data.object);

        case JSON_ARRAY:
            return JsonArray_Equals(value->data.array, other->data.array);

        case JSON_BOOLEAN:
            return JsonBoolean_Equals(value->data.boolean, other->data.boolean);

        case JSON_UNDEFINED:
        case JSON_NULL:
            return true;

        default:
            return false;
    }
}
