/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_token.h>
#include <tinycore/tiny_alloc.h>

JsonToken *JsonToken_New(JsonTokenType type, size_t offset, size_t length)
{
    JsonToken *token = (JsonToken *)tiny_malloc(sizeof(JsonToken));

    if (token == NULL)
    {
        return NULL;
    }

    token->type = type;
    token->offset = offset;
    token->length = length;

    return token;
}

void JsonToken_Delete(JsonToken *token)
{
    tiny_free(token);
}

const char *JsonTokenType_ToString(JsonTokenType type)
{
    switch (type)
    {
        case JSON_TOKEN_OBJECT_START: return "{";
        case JSON_TOKEN_OBJECT_END:   return "}";
        case JSON_TOKEN_ARRAY_START:  return "[";
        case JSON_TOKEN_ARRAY_END:    return "]";
        case JSON_TOKEN_COMMA:        return ",";
        case JSON_TOKEN_COLON:        return ":";
        case JSON_TOKEN_NULL:         return "null";
        case JSON_TOKEN_TRUE:         return "true";
        case JSON_TOKEN_FALSE:        return "false";
        case JSON_TOKEN_STRING:       return "string";
        case JSON_TOKEN_NUMBER:       return "number";
        default:                      return "invalid";
    }
}
