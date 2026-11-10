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
#include <tinycore/json_decoder.h>
#include <tinycore/json_encoder.h>
#include <tinycore/json_number.h>
#include <tinycore/json_object.h>
#include <tinycore/json_string.h>
#include <tinycore/json_value.h>
#include <tinycore/tiny_alloc.h>

static void json_object_delete_value(void *value, void *context)
{
    TINYCORE_UNUSED(context);
    JsonValue_Delete((JsonValue *)value);
}

TINYCORE_RET JsonObject_Construct(JsonObject *object)
{
    if (object == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    TinyMap_Construct(&object->data);
    TinyMap_SetDeleteListener(&object->data, json_object_delete_value, NULL);

    return TINY_RET_OK;
}

void JsonObject_Dispose(JsonObject *object)
{
    if (object == NULL)
    {
        return;
    }

    TinyMap_Dispose(&object->data);
}

JsonObject *JsonObject_New(void)
{
    JsonObject *object = (JsonObject *)tiny_malloc(sizeof(JsonObject));

    if (object == NULL)
    {
        return NULL;
    }

    if (JsonObject_Construct(object) != TINY_RET_OK)
    {
        tiny_free(object);
        return NULL;
    }

    return object;
}

JsonObject *JsonObject_NewFrom(const JsonObject *other)
{
    JsonObject *object = NULL;
    const TinyNode *node = NULL;

    if (other == NULL)
    {
        return NULL;
    }

    object = JsonObject_New();
    if (object == NULL)
    {
        return NULL;
    }

    for (node = other->data.items.first; node != NULL; node = node->next)
    {
        const TinyMapItem *item = (const TinyMapItem *)node->data;
        JsonValue *copy = NULL;

        if (item == NULL)
        {
            continue;
        }

        copy = JsonValue_NewFrom((const JsonValue *)item->value);
        if (copy == NULL || JsonObject_PutValue(object, item->key, copy) != TINY_RET_OK)
        {
            JsonValue_Delete(copy);
            JsonObject_Delete(object);
            return NULL;
        }
    }

    return object;
}

void JsonObject_Delete(JsonObject *object)
{
    if (object == NULL)
    {
        return;
    }

    JsonObject_Dispose(object);
    tiny_free(object);
}

uint32_t JsonObject_GetSize(const JsonObject *object)
{
    return (object != NULL) ? TinyMap_GetSize(&object->data) : 0U;
}

bool JsonObject_IsEmpty(const JsonObject *object)
{
    return (object == NULL) || TinyMap_IsEmpty(&object->data);
}

bool JsonObject_ContainsKey(const JsonObject *object, const char *key)
{
    return (object != NULL) && TinyMap_ContainsKey(&object->data, key);
}

JsonValue *JsonObject_GetValue(const JsonObject *object, const char *key)
{
    if (object == NULL)
    {
        return NULL;
    }

    return (JsonValue *)TinyMap_GetValue(&object->data, key);
}

JsonObject *JsonObject_GetObject(const JsonObject *object, const char *key)
{
    JsonValue *value = JsonObject_GetValue(object, key);

    return (value != NULL && value->type == JSON_OBJECT) ? value->data.object : NULL;
}

JsonNumber *JsonObject_GetNumber(const JsonObject *object, const char *key)
{
    JsonValue *value = JsonObject_GetValue(object, key);

    return (value != NULL && value->type == JSON_NUMBER) ? value->data.number : NULL;
}

JsonArray *JsonObject_GetArray(const JsonObject *object, const char *key)
{
    JsonValue *value = JsonObject_GetValue(object, key);

    return (value != NULL && value->type == JSON_ARRAY) ? value->data.array : NULL;
}

JsonString *JsonObject_GetString(const JsonObject *object, const char *key)
{
    JsonValue *value = JsonObject_GetValue(object, key);

    return (value != NULL && value->type == JSON_STRING) ? value->data.string : NULL;
}

JsonBoolean *JsonObject_GetBoolean(const JsonObject *object, const char *key)
{
    JsonValue *value = JsonObject_GetValue(object, key);

    return (value != NULL && value->type == JSON_BOOLEAN) ? value->data.boolean : NULL;
}

const char *JsonObject_GetStringValue(const JsonObject *object, const char *key)
{
    return JsonString_GetValue(JsonObject_GetString(object, key));
}

TINYCORE_RET JsonObject_PutValue(JsonObject *object, const char *key, JsonValue *value)
{
    void *old_value = NULL;
    TINYCORE_RET ret = TINY_RET_OK;

    if (object == NULL || key == NULL || value == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    ret = TinyMap_Put(&object->data, key, value, &old_value);
    if (ret != TINY_RET_OK)
    {
        return ret;
    }

    if (old_value != NULL)
    {
        JsonValue_Delete((JsonValue *)old_value);
    }

    return TINY_RET_OK;
}

static TINYCORE_RET json_object_put_created(JsonObject *object,
                                           const char *key,
                                           JsonValue *value)
{
    TINYCORE_RET ret = TINY_RET_OK;

    if (value == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    ret = JsonObject_PutValue(object, key, value);
    if (ret != TINY_RET_OK)
    {
        JsonValue_Delete(value);
    }

    return ret;
}

TINYCORE_RET JsonObject_PutString(JsonObject *object, const char *key, const char *value)
{
    return json_object_put_created(object, key, JsonValue_NewString(value));
}

TINYCORE_RET JsonObject_PutInteger(JsonObject *object, const char *key, int32_t value)
{
    return json_object_put_created(object, key, JsonValue_NewInteger(value));
}

TINYCORE_RET JsonObject_PutFloat(JsonObject *object, const char *key, double value)
{
    return json_object_put_created(object, key, JsonValue_NewFloat(value));
}

TINYCORE_RET JsonObject_PutBoolean(JsonObject *object, const char *key, bool value)
{
    return json_object_put_created(object, key, JsonValue_NewBoolean(value));
}

TINYCORE_RET JsonObject_PutNull(JsonObject *object, const char *key)
{
    return json_object_put_created(object, key, JsonValue_NewNull());
}

TINYCORE_RET JsonObject_PutNumber(JsonObject *object, const char *key, JsonNumber *value)
{
    return json_object_put_created(object, key, JsonValue_NewNumber(value));
}

TINYCORE_RET JsonObject_PutObject(JsonObject *object, const char *key, JsonObject *value)
{
    return json_object_put_created(object, key, JsonValue_NewObject(value));
}

TINYCORE_RET JsonObject_PutArray(JsonObject *object, const char *key, JsonArray *value)
{
    return json_object_put_created(object, key, JsonValue_NewArray(value));
}

TINYCORE_RET JsonObject_AddValue(JsonObject *object, const char *key, const JsonValue *value)
{
    JsonValue *copy = JsonValue_NewFrom(value);

    return json_object_put_created(object, key, copy);
}

TINYCORE_RET JsonObject_Remove(JsonObject *object, const char *key)
{
    if (object == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    return TinyMap_Erase(&object->data, key);
}

TINYCORE_RET JsonObject_Encode(const JsonObject *object, bool pretty, Buffer *buffer)
{
    return JsonEncoder_EncodeToBuffer(object, pretty, buffer);
}

char *JsonObject_ToString(const JsonObject *object, bool pretty)
{
    return JsonEncoder_ToString(object, pretty);
}

JsonObject *JsonObject_NewString(const char *string)
{
    JsonDecoder decoder;
    JsonObject *object = NULL;
    JsonDecoderStatus status = JSON_DECODER_STATUS_OK;

    if (string == NULL)
    {
        return NULL;
    }

    if (JsonDecoder_Construct(&decoder) != TINY_RET_OK)
    {
        return NULL;
    }

    object = JsonDecoder_Parse(&decoder, string, &status);
    JsonDecoder_Dispose(&decoder);

    return (status == JSON_DECODER_STATUS_OK) ? object : NULL;
}

JsonObject *JsonObject_NewStringFast(const char *string)
{
    return JsonObject_NewString(string);
}

bool JsonObject_Equals(const JsonObject *object, const JsonObject *other)
{
    const TinyNode *node = NULL;

    if (object == other)
    {
        return true;
    }

    if (object == NULL || other == NULL)
    {
        return false;
    }

    if (JsonObject_GetSize(object) != JsonObject_GetSize(other))
    {
        return false;
    }

    for (node = object->data.items.first; node != NULL; node = node->next)
    {
        const TinyMapItem *item = (const TinyMapItem *)node->data;
        const JsonValue *other_value = NULL;

        if (item == NULL)
        {
            continue;
        }

        other_value = JsonObject_GetValue(other, item->key);
        if (other_value == NULL)
        {
            return false;
        }

        if (!JsonValue_Equals((const JsonValue *)item->value, other_value))
        {
            return false;
        }
    }

    return true;
}
