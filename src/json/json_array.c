/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_array.h>
#include <tinycore/json_value.h>
#include <tinycore/tiny_alloc.h>

static void json_array_delete_value(void *data, void *context)
{
    TINYCORE_UNUSED(context);
    JsonValue_Delete((JsonValue *)data);
}

TINYCORE_RET JsonArray_Construct(JsonArray *array)
{
    if (array == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    TinyList_Construct(&array->values);
    TinyList_SetDeleteListener(&array->values, json_array_delete_value, NULL);

    return TINY_RET_OK;
}

void JsonArray_Dispose(JsonArray *array)
{
    if (array == NULL)
    {
        return;
    }

    TinyList_Dispose(&array->values);
}

JsonArray *JsonArray_New(void)
{
    JsonArray *array = (JsonArray *)tiny_malloc(sizeof(JsonArray));

    if (array == NULL)
    {
        return NULL;
    }

    if (JsonArray_Construct(array) != TINY_RET_OK)
    {
        tiny_free(array);
        return NULL;
    }

    return array;
}

JsonArray *JsonArray_NewFrom(const JsonArray *other)
{
    JsonArray *array = NULL;
    const TinyNode *node = NULL;

    if (other == NULL)
    {
        return NULL;
    }

    array = JsonArray_New();
    if (array == NULL)
    {
        return NULL;
    }

    for (node = other->values.first; node != NULL; node = node->next)
    {
        JsonValue *copy = JsonValue_NewFrom((const JsonValue *)node->data);

        if (copy == NULL || JsonArray_AddValue(array, copy) != TINY_RET_OK)
        {
            JsonValue_Delete(copy);
            JsonArray_Delete(array);
            return NULL;
        }
    }

    return array;
}

void JsonArray_Delete(JsonArray *array)
{
    if (array == NULL)
    {
        return;
    }

    JsonArray_Dispose(array);
    tiny_free(array);
}

TINYCORE_RET JsonArray_CopyInto(JsonArray *dst, const JsonArray *src)
{
    const TinyNode *node = NULL;

    if (dst == NULL || src == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }
    if (dst == src)
    {
        return TINY_RET_E_PARAM_INVALID;
    }

    /* Release any previous content of dst, keeping its delete listener. */
    JsonArray_Clean(dst);

    for (node = src->values.first; node != NULL; node = node->next)
    {
        JsonValue *copy = JsonValue_NewFrom((const JsonValue *)node->data);

        if (copy == NULL)
        {
            return TINY_RET_E_NO_MEMORY;
        }
        if (TinyList_AddTail(&dst->values, copy) == NULL)
        {
            JsonValue_Delete(copy);
            return TINY_RET_E_NO_MEMORY;
        }
    }

    return TINY_RET_OK;
}

TINYCORE_RET JsonArray_AddValue(JsonArray *array, JsonValue *value)
{
    if (array == NULL || value == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (value->type == JSON_UNDEFINED)
    {
        return TINY_RET_E_PARAM_INVALID;
    }

    if (TinyList_AddTail(&array->values, value) == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    return TINY_RET_OK;
}

TINYCORE_RET JsonArray_AddString(JsonArray *array, const char *value)
{
    return JsonArray_AddValue(array, JsonValue_NewString(value));
}

TINYCORE_RET JsonArray_AddInteger(JsonArray *array, int32_t value)
{
    return JsonArray_AddValue(array, JsonValue_NewInteger(value));
}

TINYCORE_RET JsonArray_AddFloat(JsonArray *array, double value)
{
    return JsonArray_AddValue(array, JsonValue_NewFloat(value));
}

TINYCORE_RET JsonArray_AddBoolean(JsonArray *array, bool value)
{
    return JsonArray_AddValue(array, JsonValue_NewBoolean(value));
}

TINYCORE_RET JsonArray_AddNull(JsonArray *array)
{
    return JsonArray_AddValue(array, JsonValue_NewNull());
}

TINYCORE_RET JsonArray_AddNumber(JsonArray *array, JsonNumber *value)
{
    return JsonArray_AddValue(array, JsonValue_NewNumber(value));
}

TINYCORE_RET JsonArray_AddObject(JsonArray *array, JsonObject *value)
{
    return JsonArray_AddValue(array, JsonValue_NewObject(value));
}

TINYCORE_RET JsonArray_AddArray(JsonArray *array, JsonArray *value)
{
    return JsonArray_AddValue(array, JsonValue_NewArray(value));
}

uint32_t JsonArray_GetSize(const JsonArray *array)
{
    return (array != NULL) ? TinyList_GetSize(&array->values) : 0U;
}

bool JsonArray_IsEmpty(const JsonArray *array)
{
    return (array == NULL) || TinyList_IsEmpty(&array->values);
}

JsonValue *JsonArray_GetValue(const JsonArray *array, uint32_t index)
{
    if (array == NULL)
    {
        return NULL;
    }

    return (JsonValue *)TinyList_GetAt(&array->values, index);
}

JsonString *JsonArray_GetString(const JsonArray *array, uint32_t index)
{
    JsonValue *value = JsonArray_GetValue(array, index);

    return (value != NULL && value->type == JSON_STRING) ? value->data.string : NULL;
}

JsonNumber *JsonArray_GetNumber(const JsonArray *array, uint32_t index)
{
    JsonValue *value = JsonArray_GetValue(array, index);

    return (value != NULL && value->type == JSON_NUMBER) ? value->data.number : NULL;
}

JsonObject *JsonArray_GetObject(const JsonArray *array, uint32_t index)
{
    JsonValue *value = JsonArray_GetValue(array, index);

    return (value != NULL && value->type == JSON_OBJECT) ? value->data.object : NULL;
}

JsonArray *JsonArray_GetArray(const JsonArray *array, uint32_t index)
{
    JsonValue *value = JsonArray_GetValue(array, index);

    return (value != NULL && value->type == JSON_ARRAY) ? value->data.array : NULL;
}

JsonBoolean *JsonArray_GetBoolean(const JsonArray *array, uint32_t index)
{
    JsonValue *value = JsonArray_GetValue(array, index);

    return (value != NULL && value->type == JSON_BOOLEAN) ? value->data.boolean : NULL;
}

void JsonArray_Clean(JsonArray *array)
{
    if (array == NULL)
    {
        return;
    }

    TinyList_RemoveAll(&array->values);
}

bool JsonArray_CheckValuesType(const JsonArray *array, JsonValueType type)
{
    const TinyNode *node = NULL;

    if (array == NULL)
    {
        return false;
    }

    for (node = array->values.first; node != NULL; node = node->next)
    {
        const JsonValue *value = (const JsonValue *)node->data;

        if (value == NULL || value->type != type)
        {
            return false;
        }
    }

    return true;
}

bool JsonArray_Equals(const JsonArray *array, const JsonArray *other)
{
    uint32_t size = 0;
    uint32_t i = 0;

    if (array == other)
    {
        return true;
    }

    if (array == NULL || other == NULL)
    {
        return false;
    }

    size = JsonArray_GetSize(array);
    if (size != JsonArray_GetSize(other))
    {
        return false;
    }

    for (i = 0; i < size; ++i)
    {
        if (!JsonValue_Equals(JsonArray_GetValue(array, i), JsonArray_GetValue(other, i)))
        {
            return false;
        }
    }

    return true;
}
