/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tlv8_item.h>
#include <tinycore/tiny_alloc.h>

TinyRet Tlv8Item_Construct(Tlv8Item *item)
{
    if (item == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    memset(item, 0, sizeof(Tlv8Item));

    return TINY_RET_OK;
}

void Tlv8Item_Dispose(Tlv8Item *item)
{
    if (item == NULL)
    {
        return;
    }

    tiny_free(item->data);
    memset(item, 0, sizeof(Tlv8Item));
}

Tlv8Item *Tlv8Item_New(void)
{
    Tlv8Item *item = (Tlv8Item *)tiny_malloc(sizeof(Tlv8Item));

    if (item == NULL)
    {
        return NULL;
    }

    if (Tlv8Item_Construct(item) != TINY_RET_OK)
    {
        tiny_free(item);
        return NULL;
    }

    return item;
}

void Tlv8Item_Delete(Tlv8Item *item)
{
    if (item == NULL)
    {
        return;
    }

    Tlv8Item_Dispose(item);
    tiny_free(item);
}

TinyRet Tlv8Item_Set(Tlv8Item *item,
                          uint8_t type,
                          const void *data,
                          uint32_t length)
{
    uint8_t *copy = NULL;

    if (item == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (data == NULL && length > 0)
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (length > 0)
    {
        copy = (uint8_t *)tiny_malloc(length);
        if (copy == NULL)
        {
            return TINY_RET_E_NO_MEMORY;
        }

        memcpy(copy, data, length);
    }

    tiny_free(item->data);
    item->data = copy;
    item->length = length;
    item->type = type;

    return TINY_RET_OK;
}

TinyRet Tlv8Item_Append(Tlv8Item *item,
                             const void *data,
                             uint32_t length)
{
    uint8_t *copy = NULL;
    uint32_t total = 0;

    if (item == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (length == 0)
    {
        return TINY_RET_OK;
    }

    if (data == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (length > UINT32_MAX - item->length)
    {
        return TINY_RET_E_OUT_OF_RANGE;
    }

    total = item->length + length;

    copy = (uint8_t *)tiny_malloc(total);
    if (copy == NULL)
    {
        return TINY_RET_E_NO_MEMORY;
    }

    if (item->length > 0)
    {
        memcpy(copy, item->data, item->length);
    }

    memcpy(copy + item->length, data, length);

    tiny_free(item->data);
    item->data = copy;
    item->length = total;

    return TINY_RET_OK;
}
