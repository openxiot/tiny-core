/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_time.h>
#include <tinycore/tiny_snprintf.h>

#include <errno.h>
#include <time.h>

#if defined(TINYCORE_PLATFORM_WINDOWS)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#elif defined(TINYCORE_PLATFORM_ESP32)
#  include <esp_timer.h>
#  include <freertos/FreeRTOS.h>
#  include <freertos/task.h>
#elif defined(TINYCORE_PLATFORM_POSIX)
#  include <sys/time.h>
#endif

/* Difference between the Windows FILETIME epoch (1601-01-01) and the Unix
 * epoch, in seconds. */
#define TINYCORE_FILETIME_UNIX_OFFSET 11644473600ULL

static void tiny_fill_local_time(struct tm *result)
{
#if defined(TINYCORE_PLATFORM_WINDOWS)
    time_t now = time(NULL);
    struct tm local_time;

    if (localtime_s(&local_time, &now) == 0)
    {
        *result = local_time;
    }
#else
    time_t now = time(NULL);
    struct tm local_time;

    if (localtime_r(&now, &local_time) != NULL)
    {
        *result = local_time;
    }
#endif
}

uint64_t tiny_current_microsecond(void)
{
#if defined(TINYCORE_PLATFORM_WINDOWS)
    FILETIME file_time;
    ULARGE_INTEGER value;

    GetSystemTimeAsFileTime(&file_time);
    value.LowPart = file_time.dwLowDateTime;
    value.HighPart = file_time.dwHighDateTime;

    /* 100 ns ticks since 1601. */
    return ((uint64_t)value.QuadPart / 10ULL) - (TINYCORE_FILETIME_UNIX_OFFSET * 1000000ULL);
#elif defined(TINYCORE_PLATFORM_ESP32)
    return (uint64_t)esp_timer_get_time();
#elif defined(TINYCORE_PLATFORM_POSIX)
    struct timeval now;

    if (gettimeofday(&now, NULL) != 0)
    {
        return 0;
    }

    return ((uint64_t)now.tv_sec * 1000000ULL) + (uint64_t)now.tv_usec;
#else
    return (uint64_t)time(NULL) * 1000000ULL;
#endif
}

uint64_t tiny_current_millisecond(void)
{
    return tiny_current_microsecond() / 1000ULL;
}

TINYCORE_RET tiny_getstrtime_format(char *buffer, size_t size, const char *format)
{
    struct tm local_time;
    size_t written = 0;

    RETURN_VAL_IF_FAIL(buffer != NULL, TINY_RET_E_NULL_PTR);
    RETURN_VAL_IF_FAIL(size > 0, TINY_RET_E_PARAM_INVALID);
    RETURN_VAL_IF_FAIL(format != NULL, TINY_RET_E_NULL_PTR);

    buffer[0] = '\0';
    memset(&local_time, 0, sizeof(local_time));
    tiny_fill_local_time(&local_time);

    written = strftime(buffer, size, format, &local_time);
    if (written == 0)
    {
        buffer[0] = '\0';
        return TINY_RET_E_NOT_ENOUGH;
    }

    return TINY_RET_OK;
}

TINYCORE_RET tiny_getstrtime(char *buffer, size_t size)
{
    return tiny_getstrtime_format(buffer, size, "%Y-%m-%d %H:%M:%S");
}

void tiny_sleep_ms(uint32_t milliseconds)
{
    if (milliseconds == 0)
    {
        return;
    }

#if defined(TINYCORE_PLATFORM_WINDOWS)
    Sleep((DWORD)milliseconds);
#elif defined(TINYCORE_PLATFORM_ESP32)
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
#elif defined(TINYCORE_PLATFORM_POSIX)
    {
        struct timespec delay;
        delay.tv_sec = (time_t)(milliseconds / 1000U);
        delay.tv_nsec = (long)(milliseconds % 1000U) * 1000000L;

        /* Retry if interrupted by a signal. */
        while (nanosleep(&delay, &delay) != 0)
        {
            if (errno != EINTR)
            {
                break;
            }
        }
    }
#else
    {
        clock_t start = clock();
        clock_t ticks = (clock_t)(((double)milliseconds * (double)CLOCKS_PER_SEC) / 1000.0);

        while ((clock() - start) < ticks)
        {
            /* busy wait */
        }
    }
#endif
}

void tiny_sleep_us(uint32_t microseconds)
{
    if (microseconds == 0)
    {
        return;
    }

#if defined(TINYCORE_PLATFORM_WINDOWS)
    Sleep((DWORD)((microseconds + 999U) / 1000U));
#elif defined(TINYCORE_PLATFORM_ESP32)
    vTaskDelay(pdMS_TO_TICKS((microseconds + 999U) / 1000U));
#elif defined(TINYCORE_PLATFORM_POSIX)
    {
        struct timespec delay;
        delay.tv_sec = (time_t)(microseconds / 1000000U);
        delay.tv_nsec = (long)(microseconds % 1000000U) * 1000L;

        while (nanosleep(&delay, &delay) != 0)
        {
            if (errno != EINTR)
            {
                break;
            }
        }
    }
#else
    tiny_sleep_ms((microseconds + 999U) / 1000U);
#endif
}
