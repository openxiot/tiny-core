/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_THREAD_H
#define TINYCORE_TINY_THREAD_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

#if TINYCORE_HAS_PTHREAD
#  include <pthread.h>
#endif

TINYCORE_BEGIN_DECLS

#define TINYCORE_THREAD_NAME_MAX 64U

typedef void *(*TinyThreadEntry)(void *context);

typedef enum TinyThreadStatus
{
    TINY_THREAD_STATUS_STOPPED = 0,
    TINY_THREAD_STATUS_RUNNING = 1
} TinyThreadStatus;

/*
 * Minimal joinable thread wrapper: pthreads on POSIX, CreateThread() on
 * Windows.  Platforms with neither report TINY_RET_E_NOT_SUPPORT.
 *
 * A thread is owned by whoever created it.  Dispose() and Delete() block
 * until a running thread finishes, so they are safe defaults; call Join()
 * yourself when you want to observe the exit.
 */
typedef struct TinyThread
{
    TinyThreadStatus status;
    TinyThreadEntry entry;
    void *context;
    char name[TINYCORE_THREAD_NAME_MAX + 1U];

#if defined(TINYCORE_PLATFORM_WINDOWS)
    void *handle; /* HANDLE */
    unsigned long thread_id;
#elif TINYCORE_HAS_PTHREAD
    pthread_t handle;
    bool joinable;
#endif
} TinyThread;

TINYCORE_API TINYCORE_RET TinyThread_Construct(TinyThread *thread,
                                               TinyThreadEntry entry,
                                               void *context,
                                               const char *name);

TINYCORE_API void TinyThread_Dispose(TinyThread *thread);

TINYCORE_API TinyThread *TinyThread_New(TinyThreadEntry entry,
                                        void *context,
                                        const char *name);

TINYCORE_API void TinyThread_Delete(TinyThread *thread);

TINYCORE_API TINYCORE_RET TinyThread_Start(TinyThread *thread);
TINYCORE_API TINYCORE_RET TinyThread_Join(TinyThread *thread);

TINYCORE_API bool TinyThread_IsRunning(const TinyThread *thread);
TINYCORE_API const char *TinyThread_GetName(const TinyThread *thread);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_THREAD_H */
