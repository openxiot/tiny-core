/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TLV8_ITEM_H
#define TINYCORE_TLV8_ITEM_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/* One logical TLV8 entry.  An entry longer than 255 bytes is split into
 * several fragments when a Tlv8List is serialized. */
typedef struct
{
    uint8_t type;
    uint32_t length;
    uint8_t *data;
} Tlv8Item;

TINYCORE_API TinyRet Tlv8Item_Construct(Tlv8Item *item);
TINYCORE_API void Tlv8Item_Dispose(Tlv8Item *item);
TINYCORE_API Tlv8Item *Tlv8Item_New(void);
TINYCORE_API void Tlv8Item_Delete(Tlv8Item *item);

/* Replace type/data with a copy of the first length bytes of data. */
TINYCORE_API TinyRet Tlv8Item_Set(Tlv8Item *item,
                                       uint8_t type,
                                       const void *data,
                                       uint32_t length);

/* Append a copy of length bytes to the item's value. */
TINYCORE_API TinyRet Tlv8Item_Append(Tlv8Item *item,
                                          const void *data,
                                          uint32_t length);

TINYCORE_END_DECLS

#endif /* TINYCORE_TLV8_ITEM_H */
