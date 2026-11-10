/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_alloc.h>
#include <tinycore/tiny_queue.h>

void TinyQueue_Construct(TinyQueue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    TinyList_Construct(&queue->list);
}

void TinyQueue_Dispose(TinyQueue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    TinyList_Dispose(&queue->list);
}

TinyQueue *TinyQueue_New(void)
{
    TinyQueue *queue = (TinyQueue *)tiny_malloc(sizeof(TinyQueue));

    if (queue == NULL)
    {
        return NULL;
    }

    TinyQueue_Construct(queue);
    return queue;
}

void TinyQueue_Delete(TinyQueue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    TinyQueue_Dispose(queue);
    tiny_free(queue);
}

uint32_t TinyQueue_GetSize(const TinyQueue *queue)
{
    return (queue != NULL) ? TinyList_GetSize(&queue->list) : 0U;
}

bool TinyQueue_IsEmpty(const TinyQueue *queue)
{
    return (queue == NULL) || TinyList_IsEmpty(&queue->list);
}

void *TinyQueue_Head(const TinyQueue *queue)
{
    return (queue != NULL) ? TinyList_GetHead(&queue->list) : NULL;
}

bool TinyQueue_Push(TinyQueue *queue, void *data)
{
    if (queue == NULL)
    {
        return false;
    }

    return TinyList_AddTail(&queue->list, data) != NULL;
}

void *TinyQueue_Pop(TinyQueue *queue)
{
    if (queue == NULL)
    {
        return NULL;
    }

    return TinyList_RemoveHead(&queue->list);
}

void TinyQueue_Clear(TinyQueue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    TinyList_RemoveAll(&queue->list);
}
