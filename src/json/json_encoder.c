/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_encoder.h>
#include <tinycore/json_array.h>
#include <tinycore/json_boolean.h>
#include <tinycore/json_number.h>
#include <tinycore/json_object.h>
#include <tinycore/json_string.h>
#include <tinycore/json_value.h>
#include <tinycore/tiny_alloc.h>
#include <tinycore/tiny_snprintf.h>

#include <float.h>

typedef struct JsonEncoderState
{
    JsonOutput output;
    void *context;
    Buffer *buffer;
    bool pretty;
    TinyRet status;
} JsonEncoderState;

static void json_encoder_indent(JsonEncoderState *state, int depth)
{
    static const char spaces[] = "                                ";
    int remaining = depth * 4;

    while (remaining > 0 && state->status == TINY_RET_OK)
    {
        int chunk = MIN(remaining, (int)(sizeof(spaces) - 1U));
        const char *data = spaces;

        if (state->output != NULL)
        {
            state->status = state->output(data, (size_t)chunk, state->context);
        }
        else if (state->buffer != NULL)
        {
            state->status = Buffer_Append(state->buffer, data, (size_t)chunk);
        }

        remaining -= chunk;
    }
}

static void json_encoder_write(JsonEncoderState *state, const char *data, size_t length)
{
    if (state->status != TINY_RET_OK || data == NULL || length == 0)
    {
        return;
    }

    if (state->output != NULL)
    {
        state->status = state->output(data, length, state->context);
    }
    else if (state->buffer != NULL)
    {
        state->status = Buffer_Append(state->buffer, data, length);
    }
}

static void json_encoder_write_cstr(JsonEncoderState *state, const char *string)
{
    if (string != NULL)
    {
        json_encoder_write(state, string, strlen(string));
    }
}

static void json_encoder_encode_string(JsonEncoderState *state,
                                       const char *value,
                                       size_t length)
{
    static const char hex[] = "0123456789abcdef";
    size_t i = 0;

    json_encoder_write(state, "\"", 1U);

    for (i = 0; i < length && state->status == TINY_RET_OK; ++i)
    {
        unsigned char c = (unsigned char)value[i];

        switch (c)
        {
            case '"':
                json_encoder_write(state, "\\\"", 2U);
                break;

            case '\\':
                json_encoder_write(state, "\\\\", 2U);
                break;

            case '\b':
                json_encoder_write(state, "\\b", 2U);
                break;

            case '\f':
                json_encoder_write(state, "\\f", 2U);
                break;

            case '\n':
                json_encoder_write(state, "\\n", 2U);
                break;

            case '\r':
                json_encoder_write(state, "\\r", 2U);
                break;

            case '\t':
                json_encoder_write(state, "\\t", 2U);
                break;

            default:
                if (c < 0x20U)
                {
                    char escape[7];

                    escape[0] = '\\';
                    escape[1] = 'u';
                    escape[2] = '0';
                    escape[3] = '0';
                    escape[4] = hex[(c >> 4) & 0x0FU];
                    escape[5] = hex[c & 0x0FU];
                    escape[6] = '\0';
                    json_encoder_write(state, escape, 6U);
                }
                else
                {
                    json_encoder_write(state, (const char *)&value[i], 1U);
                }
                break;
        }
    }

    json_encoder_write(state, "\"", 1U);
}

static void json_encoder_encode_number(JsonEncoderState *state, const JsonNumber *number)
{
    char text[64];

    text[0] = '\0';

    if (number->type == JSON_NUMBER_INTEGER)
    {
        tiny_snprintf(text, sizeof(text), "%d", (int)number->value.intValue);
    }
    else
    {
        double value = number->value.floatValue;

        /* JSON has no representation for NaN or infinity. */
        if (value != value || value > DBL_MAX || value < -DBL_MAX)
        {
            json_encoder_write_cstr(state, "null");
            return;
        }

        if (tiny_snprintf(text, sizeof(text), "%f", value) < 0)
        {
            tiny_snprintf(text, sizeof(text), "%.17g", value);
        }
    }

    json_encoder_write_cstr(state, text);
}

static void json_encoder_encode_value(JsonEncoderState *state,
                                      const JsonValue *value,
                                      int depth);

static void json_encoder_encode_object(JsonEncoderState *state,
                                       const JsonObject *object,
                                       int depth)
{
    const TinyNode *node = NULL;
    uint32_t size = 0;
    uint32_t index = 0;

    json_encoder_write(state, "{", 1U);

    size = JsonObject_GetSize(object);
    if (size == 0)
    {
        json_encoder_write(state, "}", 1U);
        return;
    }

    if (state->pretty)
    {
        json_encoder_write(state, "\n", 1U);
    }

    for (node = object->data.items.first; node != NULL; node = node->next, ++index)
    {
        const TinyMapItem *item = (const TinyMapItem *)node->data;

        if (item == NULL)
        {
            continue;
        }

        if (state->pretty)
        {
            json_encoder_indent(state, depth + 1);
        }

        json_encoder_encode_string(state, item->key, strlen(item->key));
        json_encoder_write(state, state->pretty ? ": " : ":", state->pretty ? 2U : 1U);
        json_encoder_encode_value(state, (const JsonValue *)item->value, depth + 1);

        if (index + 1U < size)
        {
            json_encoder_write(state, ",", 1U);
        }

        if (state->pretty)
        {
            json_encoder_write(state, "\n", 1U);
        }
    }

    if (state->pretty)
    {
        json_encoder_indent(state, depth);
    }

    json_encoder_write(state, "}", 1U);
}

static void json_encoder_encode_array(JsonEncoderState *state,
                                      const JsonArray *array,
                                      int depth)
{
    uint32_t size = JsonArray_GetSize(array);
    uint32_t index = 0;

    json_encoder_write(state, "[", 1U);

    if (size == 0)
    {
        json_encoder_write(state, "]", 1U);
        return;
    }

    if (state->pretty)
    {
        json_encoder_write(state, "\n", 1U);
    }

    for (index = 0; index < size; ++index)
    {
        if (state->pretty)
        {
            json_encoder_indent(state, depth + 1);
        }

        json_encoder_encode_value(state, JsonArray_GetValue(array, index), depth + 1);

        if (index + 1U < size)
        {
            json_encoder_write(state, ",", 1U);
        }

        if (state->pretty)
        {
            json_encoder_write(state, "\n", 1U);
        }
    }

    if (state->pretty)
    {
        json_encoder_indent(state, depth);
    }

    json_encoder_write(state, "]", 1U);
}

static void json_encoder_encode_value(JsonEncoderState *state,
                                      const JsonValue *value,
                                      int depth)
{
    if (state->status != TINY_RET_OK)
    {
        return;
    }

    if (value == NULL)
    {
        state->status = TINY_RET_E_PARAM_INVALID;
        return;
    }

    switch (value->type)
    {
        case JSON_STRING:
            json_encoder_encode_string(state,
                                       JsonString_GetValue(value->data.string),
                                       JsonString_GetLength(value->data.string));
            break;

        case JSON_NUMBER:
            json_encoder_encode_number(state, value->data.number);
            break;

        case JSON_OBJECT:
            json_encoder_encode_object(state, value->data.object, depth);
            break;

        case JSON_ARRAY:
            json_encoder_encode_array(state, value->data.array, depth);
            break;

        case JSON_BOOLEAN:
            json_encoder_write_cstr(state,
                                    JsonBoolean_GetValue(value->data.boolean) ? "true" : "false");
            break;

        case JSON_NULL:
            json_encoder_write_cstr(state, "null");
            break;

        case JSON_UNDEFINED:
        default:
            state->status = TINY_RET_E_PARAM_INVALID;
            break;
    }
}

static TinyRet json_encoder_run(const JsonObject *object,
                                     bool pretty,
                                     JsonOutput output,
                                     void *context,
                                     Buffer *buffer)
{
    JsonEncoderState state;

    if (object == NULL || (output == NULL && buffer == NULL))
    {
        return TINY_RET_E_NULL_PTR;
    }

    memset(&state, 0, sizeof(state));
    state.output = output;
    state.context = context;
    state.buffer = buffer;
    state.pretty = pretty;
    state.status = TINY_RET_OK;

    json_encoder_encode_object(&state, object, 0);

    return state.status;
}

TinyRet JsonEncoder_Encode(const JsonObject *object,
                                bool pretty,
                                JsonOutput output,
                                void *context)
{
    if (output == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    return json_encoder_run(object, pretty, output, context, NULL);
}

TinyRet JsonEncoder_EncodeToBuffer(const JsonObject *object,
                                        bool pretty,
                                        Buffer *buffer)
{
    if (buffer == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    Buffer_Clear(buffer);
    return json_encoder_run(object, pretty, NULL, NULL, buffer);
}

char *JsonEncoder_ToString(const JsonObject *object, bool pretty)
{
    Buffer buffer;
    TinyRet ret = TINY_RET_OK;
    char *result = NULL;

    if (object == NULL)
    {
        return NULL;
    }

    ret = Buffer_Construct(&buffer, 128U);
    if (ret != TINY_RET_OK)
    {
        return NULL;
    }

    ret = JsonEncoder_EncodeToBuffer(object, pretty, &buffer);
    if (ret != TINY_RET_OK)
    {
        Buffer_Dispose(&buffer);
        return NULL;
    }

    /* Steal the storage; it is already NUL terminated. */
    result = (char *)buffer.bytes;
    buffer.bytes = NULL;
    return result;
}
