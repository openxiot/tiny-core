/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_alloc.h>
#include <tinycore/tiny_map.h>

static void tiny_map_drop_item(TinyMap *map, TinyMapItem *item)
{
    if (item == NULL)
    {
        return;
    }

    if (map->delete_listener != NULL)
    {
        map->delete_listener(item->value, map->context);
    }

    TinyMapItem_Delete(item);
}

static TinyMapItem *tiny_map_find_item(const TinyMap *map, const char *key)
{
    const TinyNode *node = NULL;

    if (map == NULL || key == NULL)
    {
        return NULL;
    }

    for (node = map->items.first; node != NULL; node = node->next)
    {
        TinyMapItem *item = (TinyMapItem *)node->data;

        if (item != NULL && TinyMapItem_KeyEquals(item, key))
        {
            return item;
        }
    }

    return NULL;
}

void TinyMap_Construct(TinyMap *map)
{
    if (map == NULL)
    {
        return;
    }

    TinyList_Construct(&map->items);
    map->delete_listener = NULL;
    map->context = NULL;
}

void TinyMap_Clear(TinyMap *map)
{
    TinyNode *node = NULL;

    if (map == NULL)
    {
        return;
    }

    node = map->items.first;
    while (node != NULL)
    {
        TinyNode *next = node->next;

        tiny_map_drop_item(map, (TinyMapItem *)node->data);
        tiny_free(node);
        node = next;
    }

    map->items.first = NULL;
    map->items.last = NULL;
    map->items.size = 0;
}

void TinyMap_Dispose(TinyMap *map)
{
    if (map == NULL)
    {
        return;
    }

    TinyMap_Clear(map);
    map->delete_listener = NULL;
    map->context = NULL;
}

TinyMap *TinyMap_New(void)
{
    TinyMap *map = (TinyMap *)tiny_malloc(sizeof(TinyMap));

    if (map == NULL)
    {
        return NULL;
    }

    TinyMap_Construct(map);
    return map;
}

void TinyMap_Delete(TinyMap *map)
{
    if (map == NULL)
    {
        return;
    }

    TinyMap_Dispose(map);
    tiny_free(map);
}

void TinyMap_SetDeleteListener(TinyMap *map, TinyMapValueDeleteListener listener, void *context)
{
    if (map == NULL)
    {
        return;
    }

    map->delete_listener = listener;
    map->context = context;
}

uint32_t TinyMap_GetSize(const TinyMap *map)
{
    return (map != NULL) ? map->items.size : 0U;
}

bool TinyMap_IsEmpty(const TinyMap *map)
{
    return (map == NULL) || (map->items.size == 0);
}

bool TinyMap_ContainsKey(const TinyMap *map, const char *key)
{
    return tiny_map_find_item(map, key) != NULL;
}

TinyMapItem *TinyMap_GetItem(const TinyMap *map, const char *key)
{
    return tiny_map_find_item(map, key);
}

void *TinyMap_GetValue(const TinyMap *map, const char *key)
{
    TinyMapItem *item = tiny_map_find_item(map, key);
    return (item != NULL) ? item->value : NULL;
}

TinyRet TinyMap_Put(TinyMap *map, const char *key, void *value, void **old_value)
{
    TinyMapItem *item = NULL;
    TinyNode *node = NULL;

    RETURN_VAL_IF_FAIL(map != NULL, TINY_RET_E_NULL_PTR);
    RETURN_VAL_IF_FAIL(key != NULL, TINY_RET_E_PARAM_INVALID);

    item = tiny_map_find_item(map, key);
    if (item != NULL)
    {
        if (old_value != NULL)
        {
            *old_value = item->value;
        }

        item->value = value;
        return TINY_RET_OK;
    }

    item = TinyMapItem_New(key, value);
    if (item == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    node = TinyList_AddTail(&map->items, item);
    if (node == NULL)
    {
        TinyMapItem_Delete(item);
        return TINY_RET_E_NO_MEMORY;
    }

    if (old_value != NULL)
    {
        *old_value = NULL;
    }

    return TINY_RET_OK;
}

TinyRet TinyMap_Insert(TinyMap *map, const char *key, void *value)
{
    void *old_value = NULL;
    TinyRet ret = TINY_RET_OK;

    if (map == NULL || key == NULL)
    {
        return TINY_RET_E_PARAM_INVALID;
    }

    if (tiny_map_find_item(map, key) != NULL)
    {
        return TINY_RET_E_EXIST;
    }

    ret = TinyMap_Put(map, key, value, &old_value);
    return ret;
}

TinyRet TinyMap_Erase(TinyMap *map, const char *key)
{
    TinyNode *node = NULL;

    if (map == NULL || key == NULL)
    {
        return TINY_RET_E_PARAM_INVALID;
    }

    for (node = map->items.first; node != NULL; node = node->next)
    {
        TinyMapItem *item = (TinyMapItem *)node->data;

        if (item != NULL && TinyMapItem_KeyEquals(item, key))
        {
            /* Unlink first: TinyMapItem_Delete() frees the item. */
            TinyList_Remove(&map->items, item);
            tiny_map_drop_item(map, item);
            return TINY_RET_OK;
        }
    }

    return TINY_RET_E_NOT_FOUND;
}

TinyMapItem *TinyMap_GetItemAt(const TinyMap *map, uint32_t index)
{
    if (map == NULL)
    {
        return NULL;
    }

    return (TinyMapItem *)TinyList_GetAt(&map->items, index);
}

void TinyMap_Foreach(TinyMap *map, TinyMapVisitor visitor, void *context)
{
    TinyNode *node = NULL;

    if (map == NULL || visitor == NULL)
    {
        return;
    }

    for (node = map->items.first; node != NULL; )
    {
        TinyNode *next = node->next;
        TinyMapItem *item = (TinyMapItem *)node->data;

        if (item != NULL && !visitor(item->key, item->value, context))
        {
            break;
        }

        node = next;
    }
}
