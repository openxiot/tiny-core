/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TLV8_LIST_H
#define TINYCORE_TLV8_LIST_H

#include <tinycore/tiny_list.h>
#include <tinycore/tlv8_item.h>

TINYCORE_BEGIN_DECLS

/*
 * Ordered collection of TLV8 items.
 *
 * Type 0xFF is reserved as the TLV8 "separator": it is serialized as the two
 * bytes FF 00.  Values longer than 255 bytes are emitted as several fragments
 * (255 bytes each) with the same type, as RFC 8236 / HomeKit require.
 */
typedef struct
{
    TinyList items;  /* Tlv8Item* */
    uint8_t *bytes;  /* cached serialization */
    uint32_t size;
} Tlv8List;

TINYCORE_API TinyRet Tlv8List_Construct(Tlv8List *list);
TINYCORE_API void Tlv8List_Dispose(Tlv8List *list);
TINYCORE_API Tlv8List *Tlv8List_New(void);
TINYCORE_API void Tlv8List_Delete(Tlv8List *list);

/* Parse a serialized TLV8 buffer.  Existing items are kept unless Clear()
 * is called first.  When merge_values is true, fragments with the same type
 * are concatenated into one item. */
TINYCORE_API TinyRet Tlv8List_Parse(Tlv8List *list,
                                         const void *data,
                                         size_t length,
                                         bool merge_values);

/* Serialized form, rebuilt lazily after any modification.  Returns NULL
 * (and size 0) for an empty list or when allocation fails. */
TINYCORE_API const uint8_t *Tlv8List_GetBytes(Tlv8List *list);
TINYCORE_API uint32_t Tlv8List_GetSize(Tlv8List *list);

TINYCORE_API Tlv8Item *Tlv8List_Get(const Tlv8List *list, uint8_t type);
TINYCORE_API uint32_t Tlv8List_GetCount(const Tlv8List *list);
TINYCORE_API Tlv8Item *Tlv8List_GetAt(const Tlv8List *list, uint32_t index);

TINYCORE_API TinyRet Tlv8List_AddBytes(Tlv8List *list,
                                            uint8_t type,
                                            const void *value,
                                            size_t length,
                                            bool merge_value);

TINYCORE_API TinyRet Tlv8List_AddByte(Tlv8List *list,
                                           uint8_t type,
                                           uint8_t value,
                                           bool merge_value);

TINYCORE_API TinyRet Tlv8List_AddSeparator(Tlv8List *list);

/* Remove every item. */
TINYCORE_API void Tlv8List_Clear(Tlv8List *list);

TINYCORE_END_DECLS

#endif /* TINYCORE_TLV8_LIST_H */
