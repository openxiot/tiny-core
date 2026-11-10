/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_decoder.h>
#include <tinycore/json_array.h>
#include <tinycore/json_boolean.h>
#include <tinycore/json_number.h>
#include <tinycore/json_string.h>
#include <tinycore/json_tokenizer.h>
#include <tinycore/json_value.h>
#include <tinycore/tiny_alloc.h>

#include <errno.h>

#define JSON_DECODER_NUMBER_BUFFER 128U

static bool json_decoder_is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static int json_decoder_hex_value(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }

    return -1;
}

static TINYCORE_RET json_decoder_read_hex4(const char *text,
                                           size_t length,
                                           size_t *index,
                                           uint32_t *value)
{
    uint32_t result = 0;
    size_t i = 0;

    if (*index + 4U > length)
    {
        return TINY_RET_E_INVALID_DATA;
    }

    for (i = 0; i < 4U; ++i)
    {
        int digit = json_decoder_hex_value(text[*index + i]);

        if (digit < 0)
        {
            return TINY_RET_E_INVALID_DATA;
        }

        result = (result << 4) | (uint32_t)digit;
    }

    *index += 4U;
    *value = result;

    return TINY_RET_OK;
}

static TINYCORE_RET json_decoder_write_utf8(char *out, size_t capacity, size_t *offset, uint32_t codepoint)
{
    size_t o = *offset;

    if (codepoint <= 0x7FU)
    {
        if (o + 1U > capacity)
        {
            return TINY_RET_E_INVALID_DATA;
        }
        out[o++] = (char)codepoint;
    }
    else if (codepoint <= 0x7FFU)
    {
        if (o + 2U > capacity)
        {
            return TINY_RET_E_INVALID_DATA;
        }
        out[o++] = (char)(0xC0U | (codepoint >> 6));
        out[o++] = (char)(0x80U | (codepoint & 0x3FU));
    }
    else if (codepoint <= 0xFFFFU)
    {
        if (o + 3U > capacity)
        {
            return TINY_RET_E_INVALID_DATA;
        }
        out[o++] = (char)(0xE0U | (codepoint >> 12));
        out[o++] = (char)(0x80U | ((codepoint >> 6) & 0x3FU));
        out[o++] = (char)(0x80U | (codepoint & 0x3FU));
    }
    else if (codepoint <= 0x10FFFFU)
    {
        if (o + 4U > capacity)
        {
            return TINY_RET_E_INVALID_DATA;
        }
        out[o++] = (char)(0xF0U | (codepoint >> 18));
        out[o++] = (char)(0x80U | ((codepoint >> 12) & 0x3FU));
        out[o++] = (char)(0x80U | ((codepoint >> 6) & 0x3FU));
        out[o++] = (char)(0x80U | (codepoint & 0x3FU));
    }
    else
    {
        return TINY_RET_E_INVALID_DATA;
    }

    *offset = o;
    return TINY_RET_OK;
}

/* Decode the escapes of a JSON string body (the text between the quotes).
 * The output is NUL terminated and its length excludes the terminator. */
static TINYCORE_RET json_decoder_unescape(const char *text,
                                          size_t length,
                                          char **out,
                                          size_t *out_length)
{
    char *buffer = NULL;
    size_t i = 0;
    size_t o = 0;

    if (text == NULL || out == NULL || out_length == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    /* Every escape sequence produces at most as many bytes as it occupies
     * (six for \uXXXX, up to four UTF-8 bytes), so length is enough. */
    buffer = (char *)tiny_malloc(length + 1U);
    if (buffer == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    while (i < length)
    {
        char c = text[i];

        if (c != '\\')
        {
            buffer[o++] = c;
            ++i;
            continue;
        }

        if (i + 1U >= length)
        {
            tiny_free(buffer);
            return TINY_RET_E_INVALID_DATA;
        }

        ++i;
        c = text[i];

        switch (c)
        {
            case '"':
            case '\\':
            case '/':
                buffer[o++] = c;
                ++i;
                break;

            case 'b':
                buffer[o++] = '\b';
                ++i;
                break;

            case 'f':
                buffer[o++] = '\f';
                ++i;
                break;

            case 'n':
                buffer[o++] = '\n';
                ++i;
                break;

            case 'r':
                buffer[o++] = '\r';
                ++i;
                break;

            case 't':
                buffer[o++] = '\t';
                ++i;
                break;

            case 'u':
            {
                uint32_t codepoint = 0;
                TINYCORE_RET ret = TINY_RET_OK;

                ++i;
                ret = json_decoder_read_hex4(text, length, &i, &codepoint);
                if (ret != TINY_RET_OK)
                {
                    tiny_free(buffer);
                    return ret;
                }

                if (codepoint >= 0xD800U && codepoint <= 0xDBFFU)
                {
                    uint32_t low = 0;

                    if (i + 2U > length || text[i] != '\\' || text[i + 1U] != 'u')
                    {
                        tiny_free(buffer);
                        return TINY_RET_E_INVALID_DATA;
                    }

                    i += 2U;
                    ret = json_decoder_read_hex4(text, length, &i, &low);
                    if (ret != TINY_RET_OK || low < 0xDC00U || low > 0xDFFFU)
                    {
                        tiny_free(buffer);
                        return TINY_RET_E_INVALID_DATA;
                    }

                    codepoint = 0x10000U + ((codepoint - 0xD800U) << 10) + (low - 0xDC00U);
                }
                else if (codepoint >= 0xDC00U && codepoint <= 0xDFFFU)
                {
                    tiny_free(buffer);
                    return TINY_RET_E_INVALID_DATA;
                }

                ret = json_decoder_write_utf8(buffer, length, &o, codepoint);
                if (ret != TINY_RET_OK)
                {
                    tiny_free(buffer);
                    return ret;
                }
                break;
            }

            default:
                tiny_free(buffer);
                return TINY_RET_E_INVALID_DATA;
        }
    }

    buffer[o] = '\0';
    *out = buffer;
    *out_length = o;

    return TINY_RET_OK;
}

static TINYCORE_RET json_decoder_parse_number(const char *text,
                                              size_t length,
                                              JsonNumber **out)
{
    char buffer[JSON_DECODER_NUMBER_BUFFER];
    bool is_float = false;
    size_t i = 0;

    if (text == NULL || out == NULL || length == 0 || length >= sizeof(buffer))
    {
        return TINY_RET_E_INVALID_DATA;
    }

    memcpy(buffer, text, length);
    buffer[length] = '\0';

    for (i = 0; i < length; ++i)
    {
        if (buffer[i] == '.' || buffer[i] == 'e' || buffer[i] == 'E')
        {
            is_float = true;
            break;
        }
    }

    if (!is_float)
    {
        char *end = NULL;
        long long value = 0;

        errno = 0;
        value = strtoll(buffer, &end, 10);

        if (end == buffer + length && errno == 0 &&
            value >= (long long)INT32_MIN && value <= (long long)INT32_MAX)
        {
            *out = JsonNumber_NewInteger((int32_t)value);
            return (*out != NULL) ? TINY_RET_OK : TINY_RET_E_NO_MEMORY;
        }
    }

    {
        char *end = NULL;
        double value = 0.0;

        errno = 0;
        value = strtod(buffer, &end);

        if (end != buffer + length)
        {
            return TINY_RET_E_INVALID_DATA;
        }

        *out = JsonNumber_NewFloat(value);
        return (*out != NULL) ? TINY_RET_OK : TINY_RET_E_NO_MEMORY;
    }
}

static TINYCORE_RET json_decoder_parse_object(JsonTokenizer *tokenizer,
                                              JsonObject **out);
static TINYCORE_RET json_decoder_parse_array(JsonTokenizer *tokenizer,
                                             JsonArray **out);

static TINYCORE_RET json_decoder_parse_string_value(JsonTokenizer *tokenizer,
                                                    JsonToken *token,
                                                    JsonValue **out)
{
    char *text = NULL;
    size_t text_length = 0;
    JsonString *string = NULL;
    JsonValue *value = NULL;
    TINYCORE_RET ret = TINY_RET_OK;

    ret = json_decoder_unescape(tokenizer->string + token->offset + 1U,
                                token->length - 2U,
                                &text,
                                &text_length);
    if (ret != TINY_RET_OK)
    {
        return ret;
    }

    string = JsonString_NewLength(text, text_length);
    tiny_free(text);

    if (string == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    JsonTokenizer_Pop(tokenizer);

    value = JsonValue_NewValue(JSON_STRING, string);
    if (value == NULL)
    {
        JsonString_Delete(string);
        return TINY_RET_E_NO_MEMORY;
    }

    *out = value;
    return TINY_RET_OK;
}

static TINYCORE_RET json_decoder_parse_number_value(JsonTokenizer *tokenizer,
                                                    JsonToken *token,
                                                    JsonValue **out)
{
    JsonNumber *number = NULL;
    JsonValue *value = NULL;
    TINYCORE_RET ret = TINY_RET_OK;

    ret = json_decoder_parse_number(tokenizer->string + token->offset,
                                    token->length,
                                    &number);
    if (ret != TINY_RET_OK)
    {
        return ret;
    }

    JsonTokenizer_Pop(tokenizer);

    value = JsonValue_NewValue(JSON_NUMBER, number);
    if (value == NULL)
    {
        JsonNumber_Delete(number);
        return TINY_RET_E_NO_MEMORY;
    }

    *out = value;
    return TINY_RET_OK;
}

static TINYCORE_RET json_decoder_decode_value(JsonTokenizer *tokenizer, JsonValue **out)
{
    JsonToken *token = JsonTokenizer_Head(tokenizer);

    if (token == NULL)
    {
        return TINY_RET_E_PARSE_FAIL;
    }

    switch (token->type)
    {
        case JSON_TOKEN_OBJECT_START:
        {
            JsonObject *object = NULL;
            JsonValue *value = NULL;
            TINYCORE_RET ret = json_decoder_parse_object(tokenizer, &object);

            if (ret != TINY_RET_OK)
            {
                return ret;
            }

            value = JsonValue_NewObject(object);
            if (value == NULL)
            {
                return TINY_RET_E_NO_MEMORY;
            }

            *out = value;
            return TINY_RET_OK;
        }

        case JSON_TOKEN_ARRAY_START:
        {
            JsonArray *array = NULL;
            JsonValue *value = NULL;
            TINYCORE_RET ret = json_decoder_parse_array(tokenizer, &array);

            if (ret != TINY_RET_OK)
            {
                return ret;
            }

            value = JsonValue_NewArray(array);
            if (value == NULL)
            {
                return TINY_RET_E_NO_MEMORY;
            }

            *out = value;
            return TINY_RET_OK;
        }

        case JSON_TOKEN_STRING:
            return json_decoder_parse_string_value(tokenizer, token, out);

        case JSON_TOKEN_NUMBER:
            return json_decoder_parse_number_value(tokenizer, token, out);

        case JSON_TOKEN_TRUE:
        case JSON_TOKEN_FALSE:
        {
            JsonValue *value = JsonValue_NewBoolean(token->type == JSON_TOKEN_TRUE);

            if (value == NULL)
            {
                return TINY_RET_E_NO_MEMORY;
            }

            JsonTokenizer_Pop(tokenizer);
            *out = value;
            return TINY_RET_OK;
        }

        case JSON_TOKEN_NULL:
        {
            JsonValue *value = JsonValue_NewNull();

            if (value == NULL)
            {
                return TINY_RET_E_NO_MEMORY;
            }

            JsonTokenizer_Pop(tokenizer);
            *out = value;
            return TINY_RET_OK;
        }

        default:
            return TINY_RET_E_PARSE_FAIL;
    }
}

static TINYCORE_RET json_decoder_parse_object(JsonTokenizer *tokenizer, JsonObject **out)
{
    JsonObject *object = NULL;
    JsonToken *token = JsonTokenizer_Head(tokenizer);

    if (token == NULL || token->type != JSON_TOKEN_OBJECT_START)
    {
        return TINY_RET_E_PARSE_FAIL;
    }

    JsonTokenizer_Pop(tokenizer);

    object = JsonObject_New();
    if (object == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    token = JsonTokenizer_Head(tokenizer);
    if (token != NULL && token->type == JSON_TOKEN_OBJECT_END)
    {
        JsonTokenizer_Pop(tokenizer);
        *out = object;
        return TINY_RET_OK;
    }

    while (true)
    {
        char *key = NULL;
        size_t key_length = 0;
        JsonValue *value = NULL;
        TINYCORE_RET ret = TINY_RET_OK;

        token = JsonTokenizer_Head(tokenizer);
        if (token == NULL || token->type != JSON_TOKEN_STRING)
        {
            JsonObject_Delete(object);
            return TINY_RET_E_PARSE_FAIL;
        }

        ret = json_decoder_unescape(tokenizer->string + token->offset + 1U,
                                    token->length - 2U,
                                    &key,
                                    &key_length);
        if (ret != TINY_RET_OK)
        {
            JsonObject_Delete(object);
            return ret;
        }

        JsonTokenizer_Pop(tokenizer);

        if (memchr(key, '\0', key_length) != NULL)
        {
            /* C strings cannot represent a key with an embedded NUL. */
            tiny_free(key);
            JsonObject_Delete(object);
            return TINY_RET_E_INVALID_DATA;
        }

        token = JsonTokenizer_Head(tokenizer);
        if (token == NULL || token->type != JSON_TOKEN_COLON)
        {
            tiny_free(key);
            JsonObject_Delete(object);
            return TINY_RET_E_PARSE_FAIL;
        }

        JsonTokenizer_Pop(tokenizer);

        ret = json_decoder_decode_value(tokenizer, &value);
        if (ret != TINY_RET_OK)
        {
            tiny_free(key);
            JsonObject_Delete(object);
            return ret;
        }

        ret = JsonObject_PutValue(object, key, value);
        tiny_free(key);

        if (ret != TINY_RET_OK)
        {
            JsonValue_Delete(value);
            JsonObject_Delete(object);
            return ret;
        }

        token = JsonTokenizer_Head(tokenizer);
        if (token == NULL)
        {
            JsonObject_Delete(object);
            return TINY_RET_E_PARSE_FAIL;
        }

        if (token->type == JSON_TOKEN_COMMA)
        {
            JsonTokenizer_Pop(tokenizer);
            continue;
        }

        if (token->type == JSON_TOKEN_OBJECT_END)
        {
            JsonTokenizer_Pop(tokenizer);
            *out = object;
            return TINY_RET_OK;
        }

        JsonObject_Delete(object);
        return TINY_RET_E_PARSE_FAIL;
    }
}

static TINYCORE_RET json_decoder_parse_array(JsonTokenizer *tokenizer, JsonArray **out)
{
    JsonArray *array = NULL;
    JsonToken *token = JsonTokenizer_Head(tokenizer);

    if (token == NULL || token->type != JSON_TOKEN_ARRAY_START)
    {
        return TINY_RET_E_PARSE_FAIL;
    }

    JsonTokenizer_Pop(tokenizer);

    array = JsonArray_New();
    if (array == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    token = JsonTokenizer_Head(tokenizer);
    if (token != NULL && token->type == JSON_TOKEN_ARRAY_END)
    {
        JsonTokenizer_Pop(tokenizer);
        *out = array;
        return TINY_RET_OK;
    }

    while (true)
    {
        JsonValue *value = NULL;
        TINYCORE_RET ret = json_decoder_decode_value(tokenizer, &value);

        if (ret != TINY_RET_OK)
        {
            JsonArray_Delete(array);
            return ret;
        }

        ret = JsonArray_AddValue(array, value);
        if (ret != TINY_RET_OK)
        {
            JsonValue_Delete(value);
            JsonArray_Delete(array);
            return ret;
        }

        token = JsonTokenizer_Head(tokenizer);
        if (token == NULL)
        {
            JsonArray_Delete(array);
            return TINY_RET_E_PARSE_FAIL;
        }

        if (token->type == JSON_TOKEN_COMMA)
        {
            JsonTokenizer_Pop(tokenizer);
            continue;
        }

        if (token->type == JSON_TOKEN_ARRAY_END)
        {
            JsonTokenizer_Pop(tokenizer);
            *out = array;
            return TINY_RET_OK;
        }

        JsonArray_Delete(array);
        return TINY_RET_E_PARSE_FAIL;
    }
}

/* Locate the end of the first complete top level object. */
static JsonDecoderStatus json_decoder_scan(const char *data,
                                           size_t length,
                                           size_t *consumed,
                                           bool require_end)
{
    size_t i = 0;
    size_t depth = 0;
    bool in_string = false;
    bool escaped = false;
    bool closed = false;

    while (i < length && json_decoder_is_whitespace(data[i]))
    {
        ++i;
    }

    if (i >= length)
    {
        return JSON_DECODER_STATUS_INCOMPLETE;
    }

    if (data[i] != '{')
    {
        return JSON_DECODER_STATUS_INVALID;
    }

    for (; i < length; ++i)
    {
        char c = data[i];

        if (in_string)
        {
            if (escaped)
            {
                escaped = false;
            }
            else if (c == '\\')
            {
                escaped = true;
            }
            else if (c == '"')
            {
                in_string = false;
            }
            continue;
        }

        if (c == '"')
        {
            in_string = true;
        }
        else if (c == '{' || c == '[')
        {
            ++depth;
        }
        else if (c == '}' || c == ']')
        {
            if (depth == 0)
            {
                return JSON_DECODER_STATUS_INVALID;
            }

            --depth;
            if (depth == 0)
            {
                ++i;
                closed = true;
                break;
            }
        }
    }

    if (!closed)
    {
        return JSON_DECODER_STATUS_INCOMPLETE;
    }

    if (require_end)
    {
        while (i < length && json_decoder_is_whitespace(data[i]))
        {
            ++i;
        }

        if (i != length)
        {
            return JSON_DECODER_STATUS_INVALID;
        }
    }

    *consumed = i;
    return JSON_DECODER_STATUS_OK;
}

static JsonDecoderStatus json_decoder_status_from_ret(TINYCORE_RET ret)
{
    return (ret == TINY_RET_E_NO_MEMORY)
        ? JSON_DECODER_STATUS_NO_MEMORY
        : JSON_DECODER_STATUS_INVALID;
}

static JsonObject *json_decoder_decode_document(JsonDecoder *decoder,
                                                size_t length,
                                                JsonDecoderStatus *status)
{
    JsonTokenizer tokenizer;
    JsonObject *object = NULL;
    TINYCORE_RET ret = TINY_RET_OK;

    if (status != NULL)
    {
        *status = JSON_DECODER_STATUS_INVALID;
    }

    ret = JsonTokenizer_Construct(&tokenizer);
    if (ret != TINY_RET_OK)
    {
        return NULL;
    }

    ret = JsonTokenizer_Parse(&tokenizer, (const char *)decoder->input.bytes, length);
    if (ret != TINY_RET_OK)
    {
        JsonTokenizer_Dispose(&tokenizer);
        return NULL;
    }

    ret = json_decoder_parse_object(&tokenizer, &object);
    if (ret != TINY_RET_OK)
    {
        JsonTokenizer_Dispose(&tokenizer);
        if (status != NULL)
        {
            *status = json_decoder_status_from_ret(ret);
        }
        return NULL;
    }

    if (JsonTokenizer_Head(&tokenizer) != NULL)
    {
        JsonObject_Delete(object);
        object = NULL;
    }

    JsonTokenizer_Dispose(&tokenizer);

    if (status != NULL)
    {
        *status = (object != NULL) ? JSON_DECODER_STATUS_OK : JSON_DECODER_STATUS_INVALID;
    }

    return object;
}

TINYCORE_RET JsonDecoder_Construct(JsonDecoder *decoder)
{
    TINYCORE_RET ret = TINY_RET_OK;

    if (decoder == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    memset(decoder, 0, sizeof(JsonDecoder));

    ret = Buffer_Construct(&decoder->input, 128U);
    if (ret != TINY_RET_OK)
    {
        return ret;
    }

    decoder->constructed = true;
    return TINY_RET_OK;
}

void JsonDecoder_Dispose(JsonDecoder *decoder)
{
    if (decoder == NULL)
    {
        return;
    }

    Buffer_Dispose(&decoder->input);
    decoder->constructed = false;
}

JsonDecoder *JsonDecoder_New(void)
{
    JsonDecoder *decoder = (JsonDecoder *)tiny_malloc(sizeof(JsonDecoder));

    if (decoder == NULL)
    {
        return NULL;
    }

    if (JsonDecoder_Construct(decoder) != TINY_RET_OK)
    {
        tiny_free(decoder);
        return NULL;
    }

    return decoder;
}

void JsonDecoder_Delete(JsonDecoder *decoder)
{
    if (decoder == NULL)
    {
        return;
    }

    JsonDecoder_Dispose(decoder);
    tiny_free(decoder);
}

TINYCORE_RET JsonDecoder_Feed(JsonDecoder *decoder, const void *data, size_t length)
{
    if (decoder == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    return Buffer_Append(&decoder->input, data, length);
}

JsonObject *JsonDecoder_Parse(JsonDecoder *decoder, const char *string, JsonDecoderStatus *status)
{
    size_t consumed = 0;
    JsonDecoderStatus scan_status = JSON_DECODER_STATUS_INVALID;

    if (status != NULL)
    {
        *status = JSON_DECODER_STATUS_INVALID;
    }

    if (decoder == NULL || string == NULL)
    {
        return NULL;
    }

    Buffer_Clear(&decoder->input);
    if (Buffer_AppendString(&decoder->input, string) != TINY_RET_OK)
    {
        if (status != NULL)
        {
            *status = JSON_DECODER_STATUS_NO_MEMORY;
        }
        return NULL;
    }

    scan_status = json_decoder_scan((const char *)decoder->input.bytes,
                                    decoder->input.length,
                                    &consumed,
                                    true);
    if (status != NULL)
    {
        *status = scan_status;
    }

    if (scan_status != JSON_DECODER_STATUS_OK)
    {
        return NULL;
    }

    return json_decoder_decode_document(decoder, consumed, status);
}

JsonObject *JsonDecoder_ParseBuffered(JsonDecoder *decoder, JsonDecoderStatus *status)
{
    size_t consumed = 0;
    JsonDecoderStatus scan_status = JSON_DECODER_STATUS_INVALID;
    JsonObject *object = NULL;

    if (status != NULL)
    {
        *status = JSON_DECODER_STATUS_INVALID;
    }

    if (decoder == NULL)
    {
        return NULL;
    }

    if (decoder->input.length == 0)
    {
        if (status != NULL)
        {
            *status = JSON_DECODER_STATUS_INCOMPLETE;
        }
        return NULL;
    }

    scan_status = json_decoder_scan((const char *)decoder->input.bytes,
                                    decoder->input.length,
                                    &consumed,
                                    false);

    if (scan_status == JSON_DECODER_STATUS_INCOMPLETE)
    {
        if (status != NULL)
        {
            *status = scan_status;
        }
        return NULL;
    }

    if (scan_status != JSON_DECODER_STATUS_OK)
    {
        /* Drop the offending bytes so the next call starts clean. */
        Buffer_Clear(&decoder->input);
        if (status != NULL)
        {
            *status = scan_status;
        }
        return NULL;
    }

    object = json_decoder_decode_document(decoder, consumed, status);
    if (object != NULL)
    {
        size_t remaining = decoder->input.length - consumed;

        if (remaining > 0)
        {
            memmove(decoder->input.bytes, decoder->input.bytes + consumed, remaining);
        }

        decoder->input.length = remaining;
        decoder->input.bytes[remaining] = '\0';
    }
    else
    {
        Buffer_Clear(&decoder->input);
    }

    return object;
}

JsonObject *JsonDecoder_ParseBuffer(JsonDecoder *decoder,
                                    CycleBuffer *buffer,
                                    JsonDecoderStatus *status)
{
    uint8_t chunk[256];

    if (status != NULL)
    {
        *status = JSON_DECODER_STATUS_INVALID;
    }

    if (decoder == NULL || buffer == NULL)
    {
        return NULL;
    }

    while (CycleBuffer_GetLength(buffer) > 0)
    {
        size_t count = CycleBuffer_Read(buffer, chunk, sizeof(chunk));

        if (count == 0)
        {
            break;
        }

        if (JsonDecoder_Feed(decoder, chunk, count) != TINY_RET_OK)
        {
            if (status != NULL)
            {
                *status = JSON_DECODER_STATUS_NO_MEMORY;
            }
            return NULL;
        }
    }

    return JsonDecoder_ParseBuffered(decoder, status);
}
