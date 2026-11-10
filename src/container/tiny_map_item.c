/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_map_item.h>
#include <tinycore/tiny_strdup.h>

TinyMapItem *TinyMapItem_New(const char *key, void *value)
{
    TinyMapItem *item = NULL;

    if (key == NULL)
    {
        return NULL;
    }

    item = (TinyMapItem *)tiny_malloc(sizeof(TinyMapItem));
    if (item == NULL)
    {
        return NULL;
    }

    item->key = tiny_strdup(key);
    if (item->key == NULL)
    {
        tiny_free(item);
        return NULL;
    }

    item->value = value;
    return item;
}

void TinyMapItem_Delete(TinyMapItem *item)
{
    if (item == NULL)
    {
        return;
    }

    tiny_free(item->key);
    item->key = NULL;
    item->value = NULL;
    tiny_free(item);
}

const char *TinyMapItem_GetKey(const TinyMapItem *item)
{
    return (item != NULL) ? item->key : NULL;
}

void *TinyMapItem_GetValue(const TinyMapItem *item)
{
    return (item != NULL) ? item->value : NULL;
}

void TinyMapItem_SetValue(TinyMapItem *item, void *value)
{
    if (item != NULL)
    {
        item->value = value;
    }
}

bool TinyMapItem_KeyEquals(const TinyMapItem *item, const char *key)
{
    if (item == NULL || item->key == NULL || key == NULL)
    {
        return false;
    }

    return STR_EQUAL(item->key, key);
}
