/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tlv8_item_factory.h>
#include <tinycore/ieee754.h>
#include <tinycore/json_boolean.h>
#include <tinycore/json_number.h>
#include <tinycore/json_string.h>
#include <tinycore/json_value.h>

static TINYCORE_RET tlv8_item_set_string(Tlv8Item *item, const JsonString *string)
{
    size_t length = JsonString_GetLength(string);

    if (length > UINT32_MAX)
    {
        return TINY_RET_E_OUT_OF_RANGE;
    }

    return Tlv8Item_Set(item,
                        (uint8_t)TLV8_VALUE_STRING,
                        JsonString_GetValue(string),
                        (uint32_t)length);
}

static TINYCORE_RET tlv8_item_set_number(Tlv8Item *item, const JsonNumber *number)
{
    if (!JsonNumber_IsInteger(number))
    {
        uint8_t bytes[4];

        tiny_ieee754_encode_f32((float)JsonNumber_GetFloat(number), bytes);

        return Tlv8Item_Set(item, (uint8_t)TLV8_VALUE_FLOAT, bytes, 4U);
    }

    {
        int32_t value = JsonNumber_GetInteger(number);

        if (value >= 0 && value <= 0xFF)
        {
            uint8_t bytes[1];

            bytes[0] = (uint8_t)value;
            return Tlv8Item_Set(item, (uint8_t)TLV8_VALUE_INTEGER, bytes, 1U);
        }

        if (value >= 0 && value <= 0xFFFF)
        {
            uint8_t bytes[2];

            bytes[0] = (uint8_t)value;
            bytes[1] = (uint8_t)((uint32_t)value >> 8);
            return Tlv8Item_Set(item, (uint8_t)TLV8_VALUE_INTEGER, bytes, 2U);
        }

        {
            uint8_t bytes[4];

            bytes[0] = (uint8_t)value;
            bytes[1] = (uint8_t)((uint32_t)value >> 8);
            bytes[2] = (uint8_t)((uint32_t)value >> 16);
            bytes[3] = (uint8_t)((uint32_t)value >> 24);
            return Tlv8Item_Set(item, (uint8_t)TLV8_VALUE_INTEGER, bytes, 4U);
        }
    }
}

static TINYCORE_RET tlv8_item_set_boolean(Tlv8Item *item, const JsonBoolean *boolean)
{
    uint8_t value = JsonBoolean_GetValue(boolean) ? 1U : 0U;

    return Tlv8Item_Set(item, (uint8_t)TLV8_VALUE_BOOLEAN, &value, 1U);
}

static TINYCORE_RET tlv8_item_set_value(Tlv8Item *item, const JsonValue *value)
{
    switch (value->type)
    {
        case JSON_STRING:
            return tlv8_item_set_string(item, value->data.string);

        case JSON_NUMBER:
            return tlv8_item_set_number(item, value->data.number);

        case JSON_BOOLEAN:
            return tlv8_item_set_boolean(item, value->data.boolean);

        default:
            return TINY_RET_E_PARAM_INVALID;
    }
}

Tlv8Item *Tlv8Item_NewFromJsonValue(const JsonValue *value)
{
    Tlv8Item *item = NULL;

    if (value == NULL)
    {
        return NULL;
    }

    item = Tlv8Item_New();
    if (item == NULL)
    {
        return NULL;
    }

    if (tlv8_item_set_value(item, value) != TINY_RET_OK)
    {
        Tlv8Item_Delete(item);
        return NULL;
    }

    return item;
}

JsonValue *JsonValue_NewFromTlv8(const Tlv8Item *item)
{
    if (item == NULL || (item->data == NULL && item->length > 0))
    {
        return NULL;
    }

    switch ((Tlv8ValueType)item->type)
    {
        case TLV8_VALUE_BOOLEAN:
            if (item->length == 1U)
            {
                return JsonValue_NewBoolean(item->data[0] != 0U);
            }
            break;

        case TLV8_VALUE_INTEGER:
        {
            uint32_t raw = 0;

            if (item->length == 1U)
            {
                raw = item->data[0];
            }
            else if (item->length == 2U)
            {
                raw = (uint32_t)item->data[0] |
                      ((uint32_t)item->data[1] << 8);
            }
            else if (item->length == 4U)
            {
                raw = (uint32_t)item->data[0] |
                      ((uint32_t)item->data[1] << 8) |
                      ((uint32_t)item->data[2] << 16) |
                      ((uint32_t)item->data[3] << 24);
            }
            else
            {
                break;
            }

            return JsonValue_NewInteger((int32_t)raw);
        }

        case TLV8_VALUE_FLOAT:
            if (item->length == 4U)
            {
                return JsonValue_NewFloat(tiny_ieee754_decode_f32(item->data));
            }
            break;

        case TLV8_VALUE_STRING:
            return JsonValue_NewStringLength((const char *)((item->data != NULL) ? item->data : (const uint8_t *)""),
                                             item->length);

        default:
            break;
    }

    return NULL;
}
