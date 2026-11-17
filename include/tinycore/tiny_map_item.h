/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_MAP_ITEM_H
#define TINYCORE_TINY_MAP_ITEM_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/*
 * One key/value pair.  The key is copied and owned by the item.  The value
 * is owned by the caller (or by the map's delete listener).
 */
typedef struct
{
    char *key;
    void *value;
} TinyMapItem;

TINYCORE_API TinyMapItem *TinyMapItem_New(const char *key, void *value);
TINYCORE_API void TinyMapItem_Delete(TinyMapItem *item);

TINYCORE_API const char *TinyMapItem_GetKey(const TinyMapItem *item);
TINYCORE_API void *TinyMapItem_GetValue(const TinyMapItem *item);
TINYCORE_API void TinyMapItem_SetValue(TinyMapItem *item, void *value);

/* Case sensitive key comparison. */
TINYCORE_API bool TinyMapItem_KeyEquals(const TinyMapItem *item, const char *key);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_MAP_ITEM_H */
