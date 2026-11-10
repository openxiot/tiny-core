/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_alloc.h>
#include <tinycore/tiny_list.h>

static TinyNode *tiny_list_make_node(void *data)
{
    TinyNode *node = (TinyNode *)tiny_malloc(sizeof(TinyNode));

    if (node == NULL)
    {
        return NULL;
    }

    node->prev = NULL;
    node->next = NULL;
    node->data = data;
    return node;
}

static void tiny_list_link_tail(TinyList *list, TinyNode *node)
{
    node->prev = list->last;
    node->next = NULL;

    if (list->last != NULL)
    {
        list->last->next = node;
    }
    else
    {
        list->first = node;
    }

    list->last = node;
    ++list->size;
}

static void tiny_list_unlink(TinyList *list, TinyNode *node)
{
    if (node->prev != NULL)
    {
        node->prev->next = node->next;
    }
    else
    {
        list->first = node->next;
    }

    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }
    else
    {
        list->last = node->prev;
    }

    if (list->size > 0)
    {
        --list->size;
    }

    node->prev = NULL;
    node->next = NULL;
}

static TinyNode *tiny_list_node_at(const TinyList *list, uint32_t index)
{
    TinyNode *node = NULL;

    if (list == NULL || index >= list->size)
    {
        return NULL;
    }

    if (index <= list->size / 2U)
    {
        uint32_t i = 0;
        node = list->first;
        for (i = 0; i < index && node != NULL; ++i)
        {
            node = node->next;
        }
    }
    else
    {
        uint32_t i = list->size - 1U;
        node = list->last;
        while (i > index && node != NULL)
        {
            node = node->prev;
            --i;
        }
    }

    return node;
}

void TinyList_Construct(TinyList *list)
{
    if (list == NULL)
    {
        return;
    }

    list->size = 0;
    list->first = NULL;
    list->last = NULL;
    list->delete_listener = NULL;
    list->context = NULL;
}

void TinyList_RemoveAll(TinyList *list)
{
    TinyNode *node = NULL;

    if (list == NULL)
    {
        return;
    }

    node = list->first;
    while (node != NULL)
    {
        TinyNode *next = node->next;

        if (list->delete_listener != NULL)
        {
            list->delete_listener(node->data, list->context);
        }

        node->prev = NULL;
        node->next = NULL;
        tiny_free(node);
        node = next;
    }

    list->first = NULL;
    list->last = NULL;
    list->size = 0;
}

void TinyList_Dispose(TinyList *list)
{
    if (list == NULL)
    {
        return;
    }

    TinyList_RemoveAll(list);
    list->delete_listener = NULL;
    list->context = NULL;
}

TinyList *TinyList_New(void)
{
    TinyList *list = (TinyList *)tiny_malloc(sizeof(TinyList));

    if (list == NULL)
    {
        return NULL;
    }

    TinyList_Construct(list);
    return list;
}

void TinyList_Delete(TinyList *list)
{
    if (list == NULL)
    {
        return;
    }

    TinyList_Dispose(list);
    tiny_free(list);
}

void TinyList_SetDeleteListener(TinyList *list, TinyListDeleteListener listener, void *context)
{
    if (list == NULL)
    {
        return;
    }

    list->delete_listener = listener;
    list->context = context;
}

uint32_t TinyList_GetSize(const TinyList *list)
{
    return (list != NULL) ? list->size : 0U;
}

bool TinyList_IsEmpty(const TinyList *list)
{
    return (list == NULL) || (list->size == 0);
}

void *TinyList_GetHead(const TinyList *list)
{
    return (list != NULL && list->first != NULL) ? list->first->data : NULL;
}

void *TinyList_GetTail(const TinyList *list)
{
    return (list != NULL && list->last != NULL) ? list->last->data : NULL;
}

TinyNode *TinyList_AddHead(TinyList *list, void *data)
{
    TinyNode *node = NULL;

    RETURN_VAL_IF_FAIL(list != NULL, NULL);

    node = tiny_list_make_node(data);
    if (node == NULL)
    {
        return NULL;
    }

    node->prev = NULL;
    node->next = list->first;

    if (list->first != NULL)
    {
        list->first->prev = node;
    }
    else
    {
        list->last = node;
    }

    list->first = node;
    ++list->size;

    return node;
}

TinyNode *TinyList_AddTail(TinyList *list, void *data)
{
    TinyNode *node = NULL;

    RETURN_VAL_IF_FAIL(list != NULL, NULL);

    node = tiny_list_make_node(data);
    if (node == NULL)
    {
        return NULL;
    }

    tiny_list_link_tail(list, node);
    return node;
}

TinyNode *TinyList_InsertAfter(TinyList *list, TinyNode *node, void *data)
{
    TinyNode *new_node = NULL;

    RETURN_VAL_IF_FAIL(list != NULL, NULL);
    RETURN_VAL_IF_FAIL(node != NULL, NULL);

    new_node = tiny_list_make_node(data);
    if (new_node == NULL)
    {
        return NULL;
    }

    new_node->prev = node;
    new_node->next = node->next;

    if (node->next != NULL)
    {
        node->next->prev = new_node;
    }
    else
    {
        list->last = new_node;
    }

    node->next = new_node;
    ++list->size;

    return new_node;
}

TinyNode *TinyList_InsertBefore(TinyList *list, TinyNode *node, void *data)
{
    TinyNode *new_node = NULL;

    RETURN_VAL_IF_FAIL(list != NULL, NULL);
    RETURN_VAL_IF_FAIL(node != NULL, NULL);

    new_node = tiny_list_make_node(data);
    if (new_node == NULL)
    {
        return NULL;
    }

    new_node->next = node;
    new_node->prev = node->prev;

    if (node->prev != NULL)
    {
        node->prev->next = new_node;
    }
    else
    {
        list->first = new_node;
    }

    node->prev = new_node;
    ++list->size;

    return new_node;
}

void *TinyList_RemoveHead(TinyList *list)
{
    TinyNode *node = NULL;
    void *data = NULL;

    if (list == NULL || list->first == NULL)
    {
        return NULL;
    }

    node = list->first;
    data = node->data;
    tiny_list_unlink(list, node);
    tiny_free(node);

    return data;
}

void *TinyList_RemoveTail(TinyList *list)
{
    TinyNode *node = NULL;
    void *data = NULL;

    if (list == NULL || list->last == NULL)
    {
        return NULL;
    }

    node = list->last;
    data = node->data;
    tiny_list_unlink(list, node);
    tiny_free(node);

    return data;
}

TinyNode *TinyList_GetHeadPosition(const TinyList *list)
{
    return (list != NULL) ? list->first : NULL;
}

TinyNode *TinyList_GetTailPosition(const TinyList *list)
{
    return (list != NULL) ? list->last : NULL;
}

TinyNode *TinyList_GetNext(const TinyNode *node)
{
    return (node != NULL) ? node->next : NULL;
}

TinyNode *TinyList_GetPrev(const TinyNode *node)
{
    return (node != NULL) ? node->prev : NULL;
}

TinyNode *TinyList_GetPositionAt(const TinyList *list, uint32_t index)
{
    return tiny_list_node_at(list, index);
}

void *TinyList_GetAt(const TinyList *list, uint32_t index)
{
    TinyNode *node = tiny_list_node_at(list, index);
    return (node != NULL) ? node->data : NULL;
}

bool TinyList_SetAt(TinyList *list, uint32_t index, void *data, void **old_data)
{
    TinyNode *node = tiny_list_node_at(list, index);

    if (node == NULL)
    {
        return false;
    }

    if (old_data != NULL)
    {
        *old_data = node->data;
    }

    node->data = data;
    return true;
}

void *TinyList_RemoveAt(TinyList *list, uint32_t index)
{
    TinyNode *node = tiny_list_node_at(list, index);
    void *data = NULL;

    if (node == NULL)
    {
        return NULL;
    }

    data = node->data;
    tiny_list_unlink(list, node);
    tiny_free(node);

    return data;
}

void *TinyList_Remove(TinyList *list, const void *data)
{
    TinyNode *node = NULL;

    if (list == NULL)
    {
        return NULL;
    }

    for (node = list->first; node != NULL; node = node->next)
    {
        if (node->data == data)
        {
            void *removed = node->data;
            tiny_list_unlink(list, node);
            tiny_free(node);
            return removed;
        }
    }

    return NULL;
}

void TinyList_Foreach(TinyList *list, TinyListVisitor visitor, void *context)
{
    TinyNode *node = NULL;

    if (list == NULL || visitor == NULL)
    {
        return;
    }

    for (node = list->first; node != NULL; )
    {
        TinyNode *next = node->next;

        if (!visitor(node->data, context))
        {
            break;
        }

        node = next;
    }
}
