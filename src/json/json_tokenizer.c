/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_tokenizer.h>

typedef enum
{
    JSON_CHAR_SPACE = 0,
    JSON_CHAR_TAB,
    JSON_CHAR_CR,
    JSON_CHAR_LF
} JsonWhitespace;

static bool json_is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static bool json_is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static bool json_is_hex(char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

static bool json_is_delimiter(char c)
{
    return json_is_whitespace(c) || c == ',' || c == '}' || c == ']';
}

static void json_tokenizer_delete_token(void *data, void *context)
{
    TINYCORE_UNUSED(context);
    JsonToken_Delete((JsonToken *)data);
}

TINYCORE_RET JsonTokenizer_Construct(JsonTokenizer *tokenizer)
{
    if (tokenizer == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    TinyQueue_Construct(&tokenizer->queue);
    TinyList_SetDeleteListener(&tokenizer->queue.list, json_tokenizer_delete_token, NULL);
    tokenizer->string = NULL;
    tokenizer->length = 0;

    return TINY_RET_OK;
}

void JsonTokenizer_Dispose(JsonTokenizer *tokenizer)
{
    if (tokenizer == NULL)
    {
        return;
    }

    TinyQueue_Dispose(&tokenizer->queue);
    tokenizer->string = NULL;
    tokenizer->length = 0;
}

static TINYCORE_RET json_tokenizer_push(JsonTokenizer *tokenizer,
                                        JsonTokenType type,
                                        size_t offset,
                                        size_t length)
{
    JsonToken *token = JsonToken_New(type, offset, length);

    if (token == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    if (!TinyQueue_Push(&tokenizer->queue, token))
    {
        JsonToken_Delete(token);
        return TINY_RET_E_NO_MEMORY;
    }

    return TINY_RET_OK;
}

/* Returns the byte length of the UTF-8 sequence starting at p, or 0 when
 * the sequence is invalid. */
static size_t json_utf8_length(const uint8_t *p, size_t remaining)
{
    uint8_t c = p[0];
    size_t i = 0;
    size_t length = 0;

    if (c < 0x80U)
    {
        return 1;
    }

    if (c >= 0xC2U && c <= 0xDFU)
    {
        length = 2;
    }
    else if (c >= 0xE0U && c <= 0xEFU)
    {
        length = 3;
    }
    else if (c >= 0xF0U && c <= 0xF4U)
    {
        length = 4;
    }
    else
    {
        return 0;
    }

    if (remaining < length)
    {
        return 0;
    }

    for (i = 1; i < length; ++i)
    {
        if ((p[i] & 0xC0U) != 0x80U)
        {
            return 0;
        }
    }

    /* Reject overlong encodings and UTF-16 surrogates. */
    if (length == 3)
    {
        if (c == 0xE0U && p[1] < 0xA0U)
        {
            return 0;
        }
        if (c == 0xEDU && p[1] > 0x9FU)
        {
            return 0;
        }
    }
    else if (length == 4)
    {
        if (c == 0xF0U && p[1] < 0x90U)
        {
            return 0;
        }
        if (c == 0xF4U && p[1] > 0x8FU)
        {
            return 0;
        }
    }

    return length;
}

static TINYCORE_RET json_tokenizer_parse_string(JsonTokenizer *tokenizer, size_t *index)
{
    const char *string = tokenizer->string;
    size_t length = tokenizer->length;
    size_t start = *index;
    size_t i = start + 1U;

    while (i < length)
    {
        unsigned char c = (unsigned char)string[i];

        if (c == '"')
        {
            ++i;
            *index = i;
            return json_tokenizer_push(tokenizer,
                                       JSON_TOKEN_STRING,
                                       start,
                                       i - start);
        }

        if (c == '\\')
        {
            if (i + 1U >= length)
            {
                return TINY_RET_E_INVALID_DATA;
            }

            switch (string[i + 1U])
            {
                case '"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    i += 2U;
                    break;

                case 'u':
                    if (i + 5U >= length ||
                        !json_is_hex(string[i + 2U]) ||
                        !json_is_hex(string[i + 3U]) ||
                        !json_is_hex(string[i + 4U]) ||
                        !json_is_hex(string[i + 5U]))
                    {
                        return TINY_RET_E_INVALID_DATA;
                    }
                    i += 6U;
                    break;

                default:
                    return TINY_RET_E_INVALID_DATA;
            }

            continue;
        }

        if (c < 0x20U)
        {
            /* Raw control characters are not valid JSON. */
            return TINY_RET_E_INVALID_DATA;
        }

        if (c < 0x80U)
        {
            ++i;
            continue;
        }

        {
            size_t utf8_length = json_utf8_length((const uint8_t *)string + i, length - i);

            if (utf8_length == 0)
            {
                return TINY_RET_E_INVALID_DATA;
            }

            i += utf8_length;
        }
    }

    return TINY_RET_E_INVALID_DATA; /* unterminated */
}

static TINYCORE_RET json_tokenizer_parse_number(JsonTokenizer *tokenizer, size_t *index)
{
    const char *string = tokenizer->string;
    size_t length = tokenizer->length;
    size_t start = *index;
    size_t i = start;

    if (i < length && string[i] == '-')
    {
        ++i;
    }

    if (i >= length)
    {
        return TINY_RET_E_INVALID_DATA;
    }

    if (string[i] == '0')
    {
        ++i;
    }
    else if (string[i] >= '1' && string[i] <= '9')
    {
        while (i < length && json_is_digit(string[i]))
        {
            ++i;
        }
    }
    else
    {
        return TINY_RET_E_INVALID_DATA;
    }

    if (i < length && string[i] == '.')
    {
        ++i;
        if (i >= length || !json_is_digit(string[i]))
        {
            return TINY_RET_E_INVALID_DATA;
        }
        while (i < length && json_is_digit(string[i]))
        {
            ++i;
        }
    }

    if (i < length && (string[i] == 'e' || string[i] == 'E'))
    {
        ++i;
        if (i < length && (string[i] == '+' || string[i] == '-'))
        {
            ++i;
        }
        if (i >= length || !json_is_digit(string[i]))
        {
            return TINY_RET_E_INVALID_DATA;
        }
        while (i < length && json_is_digit(string[i]))
        {
            ++i;
        }
    }

    if (i < length && !json_is_delimiter(string[i]))
    {
        return TINY_RET_E_INVALID_DATA;
    }

    *index = i;
    return json_tokenizer_push(tokenizer, JSON_TOKEN_NUMBER, start, i - start);
}

static TINYCORE_RET json_tokenizer_parse_literal(JsonTokenizer *tokenizer,
                                                 size_t *index,
                                                 const char *literal,
                                                 JsonTokenType type)
{
    size_t literal_length = strlen(literal);
    size_t start = *index;

    if (tokenizer->length - start < literal_length)
    {
        return TINY_RET_E_INVALID_DATA;
    }

    if (memcmp(tokenizer->string + start, literal, literal_length) != 0)
    {
        return TINY_RET_E_INVALID_DATA;
    }

    if (start + literal_length < tokenizer->length &&
        !json_is_delimiter(tokenizer->string[start + literal_length]))
    {
        return TINY_RET_E_INVALID_DATA;
    }

    *index = start + literal_length;
    return json_tokenizer_push(tokenizer, type, start, literal_length);
}

TINYCORE_RET JsonTokenizer_Parse(JsonTokenizer *tokenizer, const char *string, size_t length)
{
    size_t i = 0;

    if (tokenizer == NULL || string == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    TinyQueue_Clear(&tokenizer->queue);
    tokenizer->string = string;
    tokenizer->length = length;

    while (i < length)
    {
        char c = string[i];
        TINYCORE_RET ret = TINY_RET_OK;

        if (json_is_whitespace(c))
        {
            ++i;
            continue;
        }

        switch (c)
        {
            case '{':
                ret = json_tokenizer_push(tokenizer, JSON_TOKEN_OBJECT_START, i, 1);
                ++i;
                break;

            case '}':
                ret = json_tokenizer_push(tokenizer, JSON_TOKEN_OBJECT_END, i, 1);
                ++i;
                break;

            case '[':
                ret = json_tokenizer_push(tokenizer, JSON_TOKEN_ARRAY_START, i, 1);
                ++i;
                break;

            case ']':
                ret = json_tokenizer_push(tokenizer, JSON_TOKEN_ARRAY_END, i, 1);
                ++i;
                break;

            case ',':
                ret = json_tokenizer_push(tokenizer, JSON_TOKEN_COMMA, i, 1);
                ++i;
                break;

            case ':':
                ret = json_tokenizer_push(tokenizer, JSON_TOKEN_COLON, i, 1);
                ++i;
                break;

            case '"':
                ret = json_tokenizer_parse_string(tokenizer, &i);
                break;

            case 'n':
                ret = json_tokenizer_parse_literal(tokenizer, &i, "null", JSON_TOKEN_NULL);
                break;

            case 't':
                ret = json_tokenizer_parse_literal(tokenizer, &i, "true", JSON_TOKEN_TRUE);
                break;

            case 'f':
                ret = json_tokenizer_parse_literal(tokenizer, &i, "false", JSON_TOKEN_FALSE);
                break;

            default:
                if (c == '-' || json_is_digit(c))
                {
                    ret = json_tokenizer_parse_number(tokenizer, &i);
                }
                else
                {
                    ret = TINY_RET_E_INVALID_DATA;
                }
                break;
        }

        if (ret != TINY_RET_OK)
        {
            TinyQueue_Clear(&tokenizer->queue);
            return ret;
        }
    }

    return TINY_RET_OK;
}

JsonToken *JsonTokenizer_Head(const JsonTokenizer *tokenizer)
{
    if (tokenizer == NULL)
    {
        return NULL;
    }

    return (JsonToken *)TinyQueue_Head(&tokenizer->queue);
}

void JsonTokenizer_Pop(JsonTokenizer *tokenizer)
{
    if (tokenizer == NULL)
    {
        return;
    }

    TinyQueue_Pop(&tokenizer->queue);
}

uint32_t JsonTokenizer_GetSize(const JsonTokenizer *tokenizer)
{
    return (tokenizer != NULL) ? TinyQueue_GetSize(&tokenizer->queue) : 0U;
}
