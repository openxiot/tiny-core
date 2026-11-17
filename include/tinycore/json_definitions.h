/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_DEFINITIONS_H
#define TINYCORE_JSON_DEFINITIONS_H

#include <tinycore/tiny_base.h>
#include <tinycore/tiny_list.h>
#include <tinycore/tiny_map.h>

TINYCORE_BEGIN_DECLS

typedef struct
{
    bool value;
} JsonBoolean;

/* value is NUL terminated; length excludes the terminator and may differ
 * from strlen(value) when the string contains an embedded NUL. */
typedef struct
{
    char *value;
    size_t length;
} JsonString;

typedef enum
{
    JSON_NUMBER_INTEGER = 0,
    JSON_NUMBER_FLOAT   = 1
} JsonNumberType;

typedef union
{
    int32_t intValue;
    double floatValue;
} JsonNumberValue;

typedef struct
{
    JsonNumberType type;
    JsonNumberValue value;
} JsonNumber;

typedef struct
{
    TinyList values; /* JsonValue* */
} JsonArray;

typedef struct
{
    TinyMap data; /* key -> JsonValue* */
} JsonObject;

typedef enum
{
    JSON_UNDEFINED = 0,
    JSON_STRING    = 1,
    JSON_NUMBER    = 2,
    JSON_OBJECT    = 3,
    JSON_ARRAY     = 4,
    JSON_BOOLEAN   = 5,
    JSON_NULL      = 6
} JsonValueType;

typedef union
{
    JsonString *string;
    JsonNumber *number;
    JsonObject *object;
    JsonArray *array;
    JsonBoolean *boolean;
} JsonValueData;

typedef struct
{
    JsonValueType type;
    JsonValueData data;
} JsonValue;

TINYCORE_API const char *JsonValueType_ToString(JsonValueType type);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_DEFINITIONS_H */
