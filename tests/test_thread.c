/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <string.h>

#include <tinycore/tiny_thread.h>

static void *counter_worker(void *context)
{
    int *counter = (int *)context;
    int i = 0;

    for (i = 0; i < 10000; ++i)
    {
        ++(*counter);
    }

    return context;
}

static void test_thread_lifecycle(void)
{
    TinyThread thread;
    int counter = 0;

    TINY_TEST_CHECK(TinyThread_Construct(&thread, counter_worker, &counter, "worker") == TINY_RET_OK);
    TINY_TEST_CHECK(strcmp(TinyThread_GetName(&thread), "worker") == 0);
    TINY_TEST_CHECK(!TinyThread_IsRunning(&thread));

    TINY_TEST_CHECK(TinyThread_Start(&thread) == TINY_RET_OK);
    TINY_TEST_CHECK(TinyThread_IsRunning(&thread));
    TINY_TEST_CHECK(TinyThread_Start(&thread) == TINY_RET_E_BUSY);

    TINY_TEST_CHECK(TinyThread_Join(&thread) == TINY_RET_OK);
    TINY_TEST_CHECK(!TinyThread_IsRunning(&thread));
    TINY_TEST_CHECK(counter == 10000);

    TINY_TEST_CHECK(TinyThread_Join(&thread) == TINY_RET_E_NOT_INIT);
    TINY_TEST_CHECK(TinyThread_Start(&thread) == TINY_RET_OK);
    TinyThread_Dispose(&thread); /* joins the running thread */
    TINY_TEST_CHECK(!TinyThread_IsRunning(&thread));
    TINY_TEST_CHECK(counter == 20000);
}

static void test_thread_heap(void)
{
    TinyThread *thread = TinyThread_New(counter_worker, NULL, NULL);
    int counter = 0;

    TINY_TEST_CHECK(thread != NULL);
    if (thread == NULL)
    {
        return;
    }

    TINY_TEST_CHECK(TinyThread_GetName(thread) != NULL);
    TinyThread_Delete(thread);

    thread = TinyThread_New(counter_worker, &counter, "heap");
    TINY_TEST_CHECK(thread != NULL);
    if (thread == NULL)
    {
        return;
    }

    TINY_TEST_CHECK(TinyThread_Start(thread) == TINY_RET_OK);
    TinyThread_Delete(thread); /* joins before freeing */
    TINY_TEST_CHECK(counter == 10000);
}

static void test_thread_arguments(void)
{
    TinyThread thread;

    TINY_TEST_CHECK(TinyThread_Construct(NULL, counter_worker, NULL, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(TinyThread_Construct(&thread, NULL, NULL, NULL) == TINY_RET_E_NULL_PTR);
    TinyThread_Dispose(NULL);
    TinyThread_Delete(NULL);

    TINY_TEST_CHECK(TinyThread_GetName(NULL) == NULL);
    TINY_TEST_CHECK(!TinyThread_IsRunning(NULL));
    TINY_TEST_CHECK(TinyThread_Start(NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(TinyThread_Join(NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(TinyThread_New(NULL, NULL, NULL) == NULL);
}

static void test_thread_names(void)
{
    TinyThread thread;
    char long_name[TINYCORE_THREAD_NAME_MAX + 64U];

    memset(long_name, 'n', sizeof(long_name) - 1U);
    long_name[sizeof(long_name) - 1U] = '\0';

    TINY_TEST_CHECK(TinyThread_Construct(&thread, counter_worker, NULL, long_name) == TINY_RET_OK);
    TINY_TEST_CHECK(strlen(TinyThread_GetName(&thread)) == TINYCORE_THREAD_NAME_MAX);
    TINY_TEST_CHECK(TinyThread_GetName(&thread)[TINYCORE_THREAD_NAME_MAX - 1U] == 'n');
    TINY_TEST_CHECK(TinyThread_GetName(&thread)[TINYCORE_THREAD_NAME_MAX] == '\0');
    TinyThread_Dispose(&thread);

    TINY_TEST_CHECK(TinyThread_Construct(&thread, counter_worker, NULL, NULL) == TINY_RET_OK);
    TINY_TEST_CHECK(TinyThread_GetName(&thread) != NULL);
    TINY_TEST_CHECK(strcmp(TinyThread_GetName(&thread), "") == 0);
    TinyThread_Dispose(&thread);
}

int main(void)
{
    TINY_TEST_BEGIN("thread");
    test_thread_lifecycle();
    test_thread_heap();
    test_thread_arguments();
    test_thread_names();
    return TINY_TEST_FINISH();
}
