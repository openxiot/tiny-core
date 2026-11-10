/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/string_array.h>
#include <tinycore/tiny_alloc.h>

static void string_array_delete_value(void *data, void *context)
{
    TINYCORE_UNUSED(context);
    tiny_free(data);
}

TINYCORE_RET StringArray_Construct(StringArray *array)
{
    if (array == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    TinyList_Construct(&array->values);
    TinyList_SetDeleteListener(&array->values, string_array_delete_value, NULL);

    return TINY_RET_OK;
}

void StringArray_Dispose(StringArray *array)
{
    if (array == NULL)
    {
        return;
    }

    TinyList_Dispose(&array->values);
    memset(array, 0, sizeof(StringArray));
}

StringArray *StringArray_New(void)
{
    StringArray *array = (StringArray *)tiny_malloc(sizeof(StringArray));

    if (array == NULL)
    {
        return NULL;
    }

    if (StringArray_Construct(array) != TINY_RET_OK)
    {
        tiny_free(array);
        return NULL;
    }

    return array;
}

void StringArray_Delete(StringArray *array)
{
    if (array == NULL)
    {
        return;
    }

    StringArray_Dispose(array);
    tiny_free(array);
}

TinyNode *StringArray_Add(StringArray *array, const char *value)
{
    char *copy = NULL;
    TinyNode *node = NULL;

    if (array == NULL || value == NULL)
    {
        return NULL;
    }

    copy = tiny_strdup(value);
    if (copy == NULL)
    {
        return NULL;
    }

    node = TinyList_AddTail(&array->values, copy);
    if (node == NULL)
    {
        tiny_free(copy);
    }

    return node;
}

static TINYCORE_RET string_array_parse(StringArray *array,
                                       const char *string,
                                       const char *separator)
{
    size_t separator_length = strlen(separator);
    const char *cursor = string;

    while (true)
    {
        const char *next = strstr(cursor, separator);
        size_t length = (next != NULL) ? (size_t)(next - cursor) : strlen(cursor);
        char *value = (char *)tiny_malloc(length + 1U);

        if (value == NULL)
        {
            return TINY_RET_E_NO_MEMORY;
        }

        memcpy(value, cursor, length);
        value[length] = '\0';

        if (TinyList_AddTail(&array->values, value) == NULL)
        {
            tiny_free(value);
            return TINY_RET_E_NO_MEMORY;
        }

        if (next == NULL)
        {
            break;
        }

        cursor = next + separator_length;
    }

    return TINY_RET_OK;
}

StringArray *StringArray_NewString(const char *string, const char *separator)
{
    StringArray *array = NULL;

    if (string == NULL || separator == NULL || separator[0] == '\0')
    {
        return NULL;
    }

    array = StringArray_New();
    if (array == NULL)
    {
        return NULL;
    }

    if (string_array_parse(array, string, separator) != TINY_RET_OK)
    {
        StringArray_Delete(array);
        return NULL;
    }

    return array;
}

uint32_t StringArray_GetSize(const StringArray *array)
{
    return (array != NULL) ? array->values.size : 0U;
}

const char *StringArray_GetAt(const StringArray *array, uint32_t index)
{
    if (array == NULL)
    {
        return NULL;
    }

    return (const char *)TinyList_GetAt(&array->values, index);
}
