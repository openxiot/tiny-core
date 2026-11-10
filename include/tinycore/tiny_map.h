/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_MAP_H
#define TINYCORE_TINY_MAP_H

#include <tinycore/tiny_list.h>
#include <tinycore/tiny_map_item.h>

TINYCORE_BEGIN_DECLS

/* Called for every value the map drops.  The listener is responsible for
 * freeing the value itself; the map frees the key. */
typedef void (*TinyMapValueDeleteListener)(void *value, void *context);

/* Visit one item; return false to stop. */
typedef bool (*TinyMapVisitor)(const char *key, void *value, void *context);

/*
 * Insertion ordered string-keyed map.  Keys are copied.  Lookup is a linear
 * scan, which is the right trade-off for the small collections the embedded
 * target deals with.
 */
typedef struct TinyMap
{
    TinyList items; /* TinyMapItem* */
    TinyMapValueDeleteListener delete_listener;
    void *context;
} TinyMap;

TINYCORE_API void TinyMap_Construct(TinyMap *map);
TINYCORE_API void TinyMap_Dispose(TinyMap *map);
TINYCORE_API TinyMap *TinyMap_New(void);
TINYCORE_API void TinyMap_Delete(TinyMap *map);

TINYCORE_API void TinyMap_SetDeleteListener(TinyMap *map,
                                            TinyMapValueDeleteListener listener,
                                            void *context);

TINYCORE_API uint32_t TinyMap_GetSize(const TinyMap *map);
TINYCORE_API bool TinyMap_IsEmpty(const TinyMap *map);

TINYCORE_API bool TinyMap_ContainsKey(const TinyMap *map, const char *key);
TINYCORE_API TinyMapItem *TinyMap_GetItem(const TinyMap *map, const char *key);
TINYCORE_API void *TinyMap_GetValue(const TinyMap *map, const char *key);

/* Insert or replace.  When the key already exists the previous value is
 * returned through old_value (if old_value is not NULL) and the caller owns
 * it; the delete listener is not invoked for replaced values. */
TINYCORE_API TINYCORE_RET TinyMap_Put(TinyMap *map, const char *key, void *value, void **old_value);

/* Insert only when the key is absent; returns TINY_RET_E_EXIST otherwise. */
TINYCORE_API TINYCORE_RET TinyMap_Insert(TinyMap *map, const char *key, void *value);

/* Remove one key.  The value is passed to the delete listener when one is
 * installed. */
TINYCORE_API TINYCORE_RET TinyMap_Erase(TinyMap *map, const char *key);

/* Remove everything, invoking the delete listener for each value. */
TINYCORE_API void TinyMap_Clear(TinyMap *map);

TINYCORE_API TinyMapItem *TinyMap_GetItemAt(const TinyMap *map, uint32_t index);

TINYCORE_API void TinyMap_Foreach(TinyMap *map, TinyMapVisitor visitor, void *context);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_MAP_H */
