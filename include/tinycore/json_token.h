/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_TOKEN_H
#define TINYCORE_JSON_TOKEN_H

#include <tinycore/tiny_base.h>

TINYCORE_BEGIN_DECLS

typedef enum JsonTokenType
{
    JSON_TOKEN_INVALID       = 0,
    JSON_TOKEN_OBJECT_START  = 1,
    JSON_TOKEN_OBJECT_END    = 2,
    JSON_TOKEN_ARRAY_START   = 3,
    JSON_TOKEN_ARRAY_END     = 4,
    JSON_TOKEN_COMMA         = 5,
    JSON_TOKEN_COLON         = 6,
    JSON_TOKEN_NULL          = 7,
    JSON_TOKEN_TRUE          = 8,
    JSON_TOKEN_FALSE         = 9,
    JSON_TOKEN_STRING        = 10,
    JSON_TOKEN_NUMBER        = 11
} JsonTokenType;

/* offset/length address the raw token inside the source string.  For
 * strings the quotes are included. */
typedef struct JsonToken
{
    JsonTokenType type;
    size_t offset;
    size_t length;
} JsonToken;

TINYCORE_API JsonToken *JsonToken_New(JsonTokenType type, size_t offset, size_t length);
TINYCORE_API void JsonToken_Delete(JsonToken *token);
TINYCORE_API const char *JsonTokenType_ToString(JsonTokenType type);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_TOKEN_H */
