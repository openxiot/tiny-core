/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_string.h>
#include <tinycore/tiny_alloc.h>

JsonString *JsonString_NewLength(const char *value, size_t length)
{
    JsonString *string = NULL;

    if (value == NULL && length > 0)
    {
        return NULL;
    }

    string = (JsonString *)tiny_malloc(sizeof(JsonString));
    if (string == NULL)
    {
        return NULL;
    }

    string->value = (char *)tiny_malloc(length + 1U);
    if (string->value == NULL)
    {
        tiny_free(string);
        return NULL;
    }

    if (length > 0)
    {
        memcpy(string->value, value, length);
    }

    string->value[length] = '\0';
    string->length = length;

    return string;
}

JsonString *JsonString_New(const char *value)
{
    if (value == NULL)
    {
        return NULL;
    }

    return JsonString_NewLength(value, strlen(value));
}

void JsonString_Delete(JsonString *string)
{
    if (string == NULL)
    {
        return;
    }

    tiny_free(string->value);
    string->value = NULL;
    string->length = 0;
    tiny_free(string);
}

bool JsonString_Equals(const JsonString *string, const JsonString *other)
{
    if (string == other)
    {
        return true;
    }

    if (string == NULL || other == NULL)
    {
        return false;
    }

    if (string->length != other->length)
    {
        return false;
    }

    if (string->length == 0)
    {
        return true;
    }

    return memcmp(string->value, other->value, string->length) == 0;
}

const char *JsonString_GetValue(const JsonString *string)
{
    return (string != NULL) ? string->value : NULL;
}

size_t JsonString_GetLength(const JsonString *string)
{
    return (string != NULL) ? string->length : 0U;
}
