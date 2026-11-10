/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TLV8_ITEM_FACTORY_H
#define TINYCORE_TLV8_ITEM_FACTORY_H

#include <tinycore/json_definitions.h>
#include <tinycore/tlv8_item.h>

TINYCORE_BEGIN_DECLS

/* TLV8 wire types used when converting JSON values.  The numeric values are
 * part of the on-the-wire format and must not change. */
typedef enum Tlv8ValueType
{
    TLV8_VALUE_UNDEFINED = 0,
    TLV8_VALUE_BOOLEAN   = 1,
    TLV8_VALUE_STRING    = 2,
    TLV8_VALUE_INTEGER   = 3,
    TLV8_VALUE_FLOAT     = 4
} Tlv8ValueType;

/* Convert a JSON scalar (boolean, string, number) into a TLV8 item.
 * Returns NULL for objects, arrays, null and on allocation failure. */
TINYCORE_API Tlv8Item *Tlv8Item_NewFromJsonValue(const JsonValue *value);

/* Convert a TLV8 item back into a JSON scalar.  Integers use the smallest
 * width that fits on the wire (1, 2 or 4 bytes), floats are 4 byte IEEE 754
 * in network byte order.  Returns NULL for unknown types or bad lengths. */
TINYCORE_API JsonValue *JsonValue_NewFromTlv8(const Tlv8Item *item);

TINYCORE_END_DECLS

#endif /* TINYCORE_TLV8_ITEM_FACTORY_H */
