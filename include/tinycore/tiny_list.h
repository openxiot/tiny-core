/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_LIST_H
#define TINYCORE_TINY_LIST_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

typedef struct TinyNode
{
    struct TinyNode *prev;
    struct TinyNode *next;
    void *data;
} TinyNode;

/* Called with the item data when the list drops it (Dispose/RemoveAll).
 * The listener is responsible for freeing the data itself. */
typedef void (*TinyListDeleteListener)(void *data, void *context);

/* Return false to stop the traversal. */
typedef bool (*TinyListVisitor)(void *data, void *context);

typedef struct TinyList
{
    uint32_t size;
    TinyNode *first;
    TinyNode *last;
    TinyListDeleteListener delete_listener;
    void *context;
} TinyList;

TINYCORE_API void TinyList_Construct(TinyList *list);
TINYCORE_API void TinyList_Dispose(TinyList *list);
TINYCORE_API TinyList *TinyList_New(void);
TINYCORE_API void TinyList_Delete(TinyList *list);

/* Install a listener invoked for every item the list drops. */
TINYCORE_API void TinyList_SetDeleteListener(TinyList *list,
                                             TinyListDeleteListener listener,
                                             void *context);

TINYCORE_API uint32_t TinyList_GetSize(const TinyList *list);
TINYCORE_API bool TinyList_IsEmpty(const TinyList *list);

TINYCORE_API void *TinyList_GetHead(const TinyList *list);
TINYCORE_API void *TinyList_GetTail(const TinyList *list);

/* Insert data and return the new node, or NULL on allocation failure. */
TINYCORE_API TinyNode *TinyList_AddHead(TinyList *list, void *data);
TINYCORE_API TinyNode *TinyList_AddTail(TinyList *list, void *data);

TINYCORE_API TinyNode *TinyList_InsertAfter(TinyList *list, TinyNode *node, void *data);
TINYCORE_API TinyNode *TinyList_InsertBefore(TinyList *list, TinyNode *node, void *data);

/* Unlink and free the node; the data is returned and stays owned by the
 * caller. */
TINYCORE_API void *TinyList_RemoveHead(TinyList *list);
TINYCORE_API void *TinyList_RemoveTail(TinyList *list);

/* Drop every node, invoking the delete listener for each item. */
TINYCORE_API void TinyList_RemoveAll(TinyList *list);

TINYCORE_API TinyNode *TinyList_GetHeadPosition(const TinyList *list);
TINYCORE_API TinyNode *TinyList_GetTailPosition(const TinyList *list);
TINYCORE_API TinyNode *TinyList_GetNext(const TinyNode *node);
TINYCORE_API TinyNode *TinyList_GetPrev(const TinyNode *node);

/* Indexed access, 0 based.  Returns NULL when out of range. */
TINYCORE_API TinyNode *TinyList_GetPositionAt(const TinyList *list, uint32_t index);
TINYCORE_API void *TinyList_GetAt(const TinyList *list, uint32_t index);

/* Replace the data at index; returns false when out of range.  The replaced
 * data is returned through old_data when old_data is not NULL. */
TINYCORE_API bool TinyList_SetAt(TinyList *list, uint32_t index, void *data, void **old_data);

/* Unlink and free the node at index; the data is returned and stays owned by
 * the caller. */
TINYCORE_API void *TinyList_RemoveAt(TinyList *list, uint32_t index);

/* Remove the first node holding data (pointer identity), returning its data
 * or NULL when not present. */
TINYCORE_API void *TinyList_Remove(TinyList *list, const void *data);

/* Visit every item.  The list must not be structurally modified from the
 * visitor. */
TINYCORE_API void TinyList_Foreach(TinyList *list, TinyListVisitor visitor, void *context);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_LIST_H */
