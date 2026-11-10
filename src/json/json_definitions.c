/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_definitions.h>

const char *JsonValueType_ToString(JsonValueType type)
{
    switch (type)
    {
        case JSON_UNDEFINED: return "undefined";
        case JSON_STRING:    return "string";
        case JSON_NUMBER:    return "number";
        case JSON_OBJECT:    return "object";
        case JSON_ARRAY:     return "array";
        case JSON_BOOLEAN:   return "boolean";
        case JSON_NULL:      return "null";
        default:             return "unknown";
    }
}
