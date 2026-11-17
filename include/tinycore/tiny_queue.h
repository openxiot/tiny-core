/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_QUEUE_H
#define TINYCORE_TINY_QUEUE_H

#include <tinycore/tiny_list.h>

TINYCORE_BEGIN_DECLS

/* FIFO queue built on TinyList.  Head is the oldest element. */
typedef struct
{
    TinyList list;
} TinyQueue;

TINYCORE_API void TinyQueue_Construct(TinyQueue *queue);
TINYCORE_API void TinyQueue_Dispose(TinyQueue *queue);
TINYCORE_API TinyQueue *TinyQueue_New(void);
TINYCORE_API void TinyQueue_Delete(TinyQueue *queue);

TINYCORE_API uint32_t TinyQueue_GetSize(const TinyQueue *queue);
TINYCORE_API bool TinyQueue_IsEmpty(const TinyQueue *queue);

/* Peek at the next element without removing it. */
TINYCORE_API void *TinyQueue_Head(const TinyQueue *queue);

/* Enqueue at the tail. */
TINYCORE_API bool TinyQueue_Push(TinyQueue *queue, void *data);

/* Dequeue the head element; the data stays owned by the caller. */
TINYCORE_API void *TinyQueue_Pop(TinyQueue *queue);

/* Drop every element, invoking the list delete listener for each. */
TINYCORE_API void TinyQueue_Clear(TinyQueue *queue);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_QUEUE_H */
