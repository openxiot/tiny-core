/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tlv8_list.h>
#include <tinycore/tiny_alloc.h>

#define TLV8_TYPE_SEPARATOR 0xFFU
#define TLV8_MAX_FRAGMENT   255U

static TINYCORE_RET tlv8_list_serialize(Tlv8List *list);

static void tlv8_list_delete_item(void *data, void *context)
{
    TINYCORE_UNUSED(context);
    Tlv8Item_Delete((Tlv8Item *)data);
}

static void tlv8_list_invalidate(Tlv8List *list)
{
    tiny_free(list->bytes);
    list->bytes = NULL;
    list->size = 0;
}

TINYCORE_RET Tlv8List_Construct(Tlv8List *list)
{
    if (list == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    memset(list, 0, sizeof(Tlv8List));
    TinyList_Construct(&list->items);
    TinyList_SetDeleteListener(&list->items, tlv8_list_delete_item, NULL);

    return TINY_RET_OK;
}

void Tlv8List_Dispose(Tlv8List *list)
{
    if (list == NULL)
    {
        return;
    }

    TinyList_Dispose(&list->items);
    tlv8_list_invalidate(list);
    memset(list, 0, sizeof(Tlv8List));
}

Tlv8List *Tlv8List_New(void)
{
    Tlv8List *list = (Tlv8List *)tiny_malloc(sizeof(Tlv8List));

    if (list == NULL)
    {
        return NULL;
    }

    if (Tlv8List_Construct(list) != TINY_RET_OK)
    {
        tiny_free(list);
        return NULL;
    }

    return list;
}

void Tlv8List_Delete(Tlv8List *list)
{
    if (list == NULL)
    {
        return;
    }

    Tlv8List_Dispose(list);
    tiny_free(list);
}

Tlv8Item *Tlv8List_Get(const Tlv8List *list, uint8_t type)
{
    const TinyNode *node = NULL;

    if (list == NULL)
    {
        return NULL;
    }

    for (node = list->items.first; node != NULL; node = node->next)
    {
        Tlv8Item *item = (Tlv8Item *)node->data;

        if (item != NULL && item->type == type)
        {
            return item;
        }
    }

    return NULL;
}

uint32_t Tlv8List_GetCount(const Tlv8List *list)
{
    return (list != NULL) ? list->items.size : 0U;
}

Tlv8Item *Tlv8List_GetAt(const Tlv8List *list, uint32_t index)
{
    if (list == NULL)
    {
        return NULL;
    }

    return (Tlv8Item *)TinyList_GetAt(&list->items, index);
}

void Tlv8List_Clear(Tlv8List *list)
{
    if (list == NULL)
    {
        return;
    }

    TinyList_RemoveAll(&list->items);
    tlv8_list_invalidate(list);
}

TINYCORE_RET Tlv8List_AddBytes(Tlv8List *list,
                               uint8_t type,
                               const void *value,
                               size_t length,
                               bool merge_value)
{
    Tlv8Item *item = NULL;
    TINYCORE_RET ret = TINY_RET_OK;

    if (list == NULL || (value == NULL && length > 0))
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (length > UINT32_MAX)
    {
        return TINY_RET_E_OUT_OF_RANGE;
    }

    if (type == TLV8_TYPE_SEPARATOR && length > 0)
    {
        return TINY_RET_E_PARAM_INVALID;
    }

    if (merge_value && type != TLV8_TYPE_SEPARATOR)
    {
        item = Tlv8List_Get(list, type);
        if (item != NULL)
        {
            ret = Tlv8Item_Append(item, value, (uint32_t)length);
            if (ret != TINY_RET_OK)
            {
                return ret;
            }

            tlv8_list_invalidate(list);
            return TINY_RET_OK;
        }
    }

    item = Tlv8Item_New();
    if (item == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    ret = Tlv8Item_Set(item, type, value, (uint32_t)length);
    if (ret != TINY_RET_OK)
    {
        Tlv8Item_Delete(item);
        return ret;
    }

    if (TinyList_AddTail(&list->items, item) == NULL)
    {
        Tlv8Item_Delete(item);
        return TINY_RET_E_NO_MEMORY;
    }

    tlv8_list_invalidate(list);

    return TINY_RET_OK;
}

TINYCORE_RET Tlv8List_AddByte(Tlv8List *list,
                              uint8_t type,
                              uint8_t value,
                              bool merge_value)
{
    return Tlv8List_AddBytes(list, type, &value, 1U, merge_value);
}

TINYCORE_RET Tlv8List_AddSeparator(Tlv8List *list)
{
    if (list == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    return Tlv8List_AddBytes(list, TLV8_TYPE_SEPARATOR, NULL, 0U, false);
}

TINYCORE_RET Tlv8List_Parse(Tlv8List *list,
                            const void *data,
                            size_t length,
                            bool merge_values)
{
    const uint8_t *input = (const uint8_t *)data;
    size_t offset = 0;

    if (list == NULL || (data == NULL && length > 0))
    {
        return TINY_RET_E_NULL_PTR;
    }

    while (offset < length)
    {
        uint8_t type = 0;
        size_t fragment_length = 0;
        TINYCORE_RET ret = TINY_RET_OK;

        if (length - offset < 2U)
        {
            return TINY_RET_E_INVALID_DATA;
        }

        type = input[offset];
        fragment_length = input[offset + 1U];

        if (fragment_length > length - offset - 2U)
        {
            return TINY_RET_E_INVALID_DATA;
        }

        ret = Tlv8List_AddBytes(list,
                                type,
                                input + offset + 2U,
                                fragment_length,
                                merge_values);
        if (ret != TINY_RET_OK)
        {
            return ret;
        }

        offset += 2U + fragment_length;
    }

    return TINY_RET_OK;
}

static size_t tlv8_item_serialized_size(const Tlv8Item *item)
{
    size_t fragments = 0;

    if (item->type == TLV8_TYPE_SEPARATOR)
    {
        return 2U;
    }

    fragments = ((size_t)item->length + (TLV8_MAX_FRAGMENT - 1U)) / TLV8_MAX_FRAGMENT;
    if (fragments == 0)
    {
        fragments = 1;
    }

    return (size_t)item->length + 2U * fragments;
}

const uint8_t *Tlv8List_GetBytes(Tlv8List *list)
{
    if (list == NULL)
    {
        return NULL;
    }

    if (list->bytes == NULL && !TinyList_IsEmpty(&list->items))
    {
        if (tlv8_list_serialize(list) != TINY_RET_OK)
        {
            return NULL;
        }
    }

    return list->bytes;
}

uint32_t Tlv8List_GetSize(Tlv8List *list)
{
    (void)Tlv8List_GetBytes(list);

    return (list != NULL) ? list->size : 0U;
}

static void tlv8_serialize_item(uint8_t *output,
                                size_t *offset,
                                const Tlv8Item *item)
{
    size_t position = 0;

    if (item->type == TLV8_TYPE_SEPARATOR)
    {
        output[(*offset)++] = TLV8_TYPE_SEPARATOR;
        output[(*offset)++] = 0;
        return;
    }

    do
    {
        size_t chunk = MIN((size_t)item->length - position, TLV8_MAX_FRAGMENT);

        output[(*offset)++] = item->type;
        output[(*offset)++] = (uint8_t)chunk;

        if (chunk > 0)
        {
            memcpy(output + *offset, item->data + position, chunk);
        }

        *offset += chunk;
        position += chunk;
    } while (position < item->length);
}

static TINYCORE_RET tlv8_list_serialize(Tlv8List *list)
{
    const TinyNode *node = NULL;
    size_t total = 0;
    size_t offset = 0;
    uint8_t *bytes = NULL;

    for (node = list->items.first; node != NULL; node = node->next)
    {
        const Tlv8Item *item = (const Tlv8Item *)node->data;

        if (item == NULL)
        {
            continue;
        }

        total += tlv8_item_serialized_size(item);
        if (total > UINT32_MAX)
        {
            return TINY_RET_E_OUT_OF_RANGE;
        }
    }

    if (total == 0)
    {
        return TINY_RET_OK;
    }

    bytes = (uint8_t *)tiny_malloc(total);
    if (bytes == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    for (node = list->items.first; node != NULL; node = node->next)
    {
        const Tlv8Item *item = (const Tlv8Item *)node->data;

        if (item == NULL)
        {
            continue;
        }

        tlv8_serialize_item(bytes, &offset, item);
    }

    tiny_free(list->bytes);
    list->bytes = bytes;
    list->size = (uint32_t)total;

    return TINY_RET_OK;
}
