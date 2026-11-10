/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_thread.h>
#include <tinycore/tiny_alloc.h>
#include <tinycore/tiny_strnlen.h>

#if defined(TINYCORE_PLATFORM_WINDOWS)
#  include <windows.h>
#elif TINYCORE_HAS_PTHREAD
#  include <errno.h>
#endif

#if defined(TINYCORE_PLATFORM_WINDOWS) || TINYCORE_HAS_PTHREAD

static void tiny_thread_trampoline(TinyThread *thread)
{
    (void)thread->entry(thread->context);
}

#endif

#if defined(TINYCORE_PLATFORM_WINDOWS)

static DWORD WINAPI tiny_thread_win_entry(LPVOID parameter)
{
    tiny_thread_trampoline((TinyThread *)parameter);
    return 0;
}

#elif TINYCORE_HAS_PTHREAD

static void *tiny_thread_posix_entry(void *parameter)
{
    tiny_thread_trampoline((TinyThread *)parameter);
    return NULL;
}

#endif

static void tiny_thread_copy_name(TinyThread *thread, const char *name)
{
    size_t length = 0;

    if (name == NULL)
    {
        thread->name[0] = '\0';
        return;
    }

    length = tiny_strnlen(name, TINYCORE_THREAD_NAME_MAX);
    memcpy(thread->name, name, length);
    thread->name[length] = '\0';
}

TINYCORE_RET TinyThread_Construct(TinyThread *thread,
                                  TinyThreadEntry entry,
                                  void *context,
                                  const char *name)
{
    if (thread == NULL || entry == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    memset(thread, 0, sizeof(TinyThread));
    thread->entry = entry;
    thread->context = context;
    thread->status = TINY_THREAD_STATUS_STOPPED;
    tiny_thread_copy_name(thread, name);

    return TINY_RET_OK;
}

TinyThread *TinyThread_New(TinyThreadEntry entry, void *context, const char *name)
{
    TinyThread *thread = (TinyThread *)tiny_malloc(sizeof(TinyThread));

    if (thread == NULL)
    {
        return NULL;
    }

    if (TinyThread_Construct(thread, entry, context, name) != TINY_RET_OK)
    {
        tiny_free(thread);
        return NULL;
    }

    return thread;
}

TINYCORE_RET TinyThread_Start(TinyThread *thread)
{
    if (thread == NULL || thread->entry == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (thread->status == TINY_THREAD_STATUS_RUNNING)
    {
        return TINY_RET_E_BUSY;
    }

#if defined(TINYCORE_PLATFORM_WINDOWS)
    thread->handle = CreateThread(NULL, 0, tiny_thread_win_entry, thread, 0U, &thread->thread_id);
    if (thread->handle == NULL)
    {
        return TINY_RET_E_UNKNOWN;
    }
#elif TINYCORE_HAS_PTHREAD
    {
        int error = pthread_create(&thread->handle, NULL, tiny_thread_posix_entry, thread);

        if (error != 0)
        {
            return TINY_RET_E_UNKNOWN;
        }

        thread->joinable = true;
    }
#else
    return TINY_RET_E_NOT_SUPPORT;
#endif

    thread->status = TINY_THREAD_STATUS_RUNNING;

    return TINY_RET_OK;
}

TINYCORE_RET TinyThread_Join(TinyThread *thread)
{
    if (thread == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

#if defined(TINYCORE_PLATFORM_WINDOWS)
    if (thread->status != TINY_THREAD_STATUS_RUNNING || thread->handle == NULL)
    {
        return TINY_RET_E_NOT_INIT;
    }

    if (WaitForSingleObject((HANDLE)thread->handle, INFINITE) != WAIT_OBJECT_0)
    {
        (void)CloseHandle((HANDLE)thread->handle);
        thread->handle = NULL;
        thread->status = TINY_THREAD_STATUS_STOPPED;
        return TINY_RET_E_UNKNOWN;
    }

    (void)CloseHandle((HANDLE)thread->handle);
    thread->handle = NULL;
    thread->status = TINY_THREAD_STATUS_STOPPED;

    return TINY_RET_OK;
#elif TINYCORE_HAS_PTHREAD
    {
        int error = 0;

        if (!thread->joinable)
        {
            return TINY_RET_E_NOT_INIT;
        }

        error = pthread_join(thread->handle, NULL);
        if (error != 0)
        {
            return (error == EDEADLK) ? TINY_RET_E_PARAM_INVALID : TINY_RET_E_UNKNOWN;
        }

        thread->joinable = false;
        thread->status = TINY_THREAD_STATUS_STOPPED;

        return TINY_RET_OK;
    }
#else
    TINYCORE_UNUSED(thread);
    return TINY_RET_E_NOT_SUPPORT;
#endif
}

void TinyThread_Dispose(TinyThread *thread)
{
    if (thread == NULL)
    {
        return;
    }

    (void)TinyThread_Join(thread);
    memset(thread, 0, sizeof(TinyThread));
}

void TinyThread_Delete(TinyThread *thread)
{
    if (thread == NULL)
    {
        return;
    }

    TinyThread_Dispose(thread);
    tiny_free(thread);
}

bool TinyThread_IsRunning(const TinyThread *thread)
{
    return (thread != NULL) && (thread->status == TINY_THREAD_STATUS_RUNNING);
}

const char *TinyThread_GetName(const TinyThread *thread)
{
    return (thread != NULL) ? thread->name : NULL;
}
