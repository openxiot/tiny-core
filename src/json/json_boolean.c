/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_boolean.h>
#include <tinycore/tiny_alloc.h>

JsonBoolean *JsonBoolean_New(bool value)
{
    JsonBoolean *boolean = (JsonBoolean *)tiny_malloc(sizeof(JsonBoolean));

    if (boolean == NULL)
    {
        return NULL;
    }

    boolean->value = value;
    return boolean;
}

void JsonBoolean_Delete(JsonBoolean *boolean)
{
    tiny_free(boolean);
}

bool JsonBoolean_Equals(const JsonBoolean *boolean, const JsonBoolean *other)
{
    if (boolean == other)
    {
        return true;
    }

    if (boolean == NULL || other == NULL)
    {
        return false;
    }

    return boolean->value == other->value;
}

bool JsonBoolean_GetValue(const JsonBoolean *boolean)
{
    return (boolean != NULL) && boolean->value;
}
