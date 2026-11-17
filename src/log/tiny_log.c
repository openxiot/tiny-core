/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_log.h>
#include <tinycore/tiny_snprintf.h>
#include <tinycore/tiny_time.h>

#include <stdio.h>
#include <string.h>

#if TINYCORE_HAS_PTHREAD
#  include <pthread.h>
#endif

#if defined(TINYCORE_PLATFORM_WINDOWS)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <io.h>
#  include <windows.h>
#elif defined(TINYCORE_PLATFORM_POSIX)
#  include <unistd.h>
#endif

#define TINYCORE_LOG_TAG_LEN   32
#define TINYCORE_LOG_LINE_LEN  512

typedef struct
{
    TinyLogLevel level;
    FILE *file;
    bool owns_file;
    bool console;
    char tag[TINYCORE_LOG_TAG_LEN];
#if TINYCORE_HAS_PTHREAD
    pthread_mutex_t mutex;
    bool mutex_ready;
#endif
} TinyLogState;

static TinyLogState g_log_state =
{
    TINY_LOG_LEVEL_INFO,
    NULL,
    false,
    false,
    "TinyCore",
#if TINYCORE_HAS_PTHREAD
    PTHREAD_MUTEX_INITIALIZER,
    false
#endif
};

static const char *tiny_log_level_name(TinyLogLevel level)
{
    switch (level)
    {
        case TINY_LOG_LEVEL_VERBOSE: return "VERBOSE";
        case TINY_LOG_LEVEL_DEBUG:   return "DEBUG";
        case TINY_LOG_LEVEL_INFO:    return "INFO";
        case TINY_LOG_LEVEL_WARN:    return "WARN";
        case TINY_LOG_LEVEL_ERROR:   return "ERROR";
        default:                     return "UNKNOWN";
    }
}

static const char *tiny_log_level_color(TinyLogLevel level)
{
#if defined(TINYCORE_PLATFORM_WINDOWS)
    TINYCORE_UNUSED(level);
    return "";
#else
    switch (level)
    {
        case TINY_LOG_LEVEL_DEBUG:   return "\033[36m";
        case TINY_LOG_LEVEL_INFO:    return "\033[32m";
        case TINY_LOG_LEVEL_WARN:    return "\033[33m";
        case TINY_LOG_LEVEL_ERROR:   return "\033[31m";
        default:                     return "";
    }
#endif
}

static bool tiny_log_console_color_supported(void)
{
#if defined(TINYCORE_PLATFORM_WINDOWS)
    HANDLE output = GetStdHandle(STD_ERROR_HANDLE);
    DWORD mode = 0;

    if (output == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    return GetConsoleMode(output, &mode) != 0;
#elif defined(TINYCORE_PLATFORM_POSIX)
    return isatty(fileno(stderr)) != 0;
#else
    return false;
#endif
}

static void tiny_log_lock(void)
{
#if TINYCORE_HAS_PTHREAD
    if (g_log_state.mutex_ready)
    {
        pthread_mutex_lock(&g_log_state.mutex);
    }
#endif
}

static void tiny_log_unlock(void)
{
#if TINYCORE_HAS_PTHREAD
    if (g_log_state.mutex_ready)
    {
        pthread_mutex_unlock(&g_log_state.mutex);
    }
#endif
}

static void tiny_log_reset(void)
{
    if (g_log_state.file != NULL && g_log_state.owns_file)
    {
        fflush(g_log_state.file);
        fclose(g_log_state.file);
    }

    g_log_state.file = NULL;
    g_log_state.owns_file = false;
    g_log_state.console = false;
}

TinyRet tiny_log_init(const char *tag, TinyLogLevel level)
{
    tiny_log_lock();
    tiny_log_reset();

    g_log_state.console = true;
    g_log_state.level = level;
    if (tag != NULL)
    {
        tiny_snprintf(g_log_state.tag, sizeof(g_log_state.tag), "%s", tag);
    }

#if TINYCORE_HAS_PTHREAD
    if (!g_log_state.mutex_ready)
    {
        if (pthread_mutex_init(&g_log_state.mutex, NULL) == 0)
        {
            g_log_state.mutex_ready = true;
        }
    }
#endif
    tiny_log_unlock();

    return TINY_RET_OK;
}

TinyRet tiny_log_init_file(const char *tag, TinyLogLevel level, const char *path)
{
    FILE *file = NULL;

    RETURN_VAL_IF_FAIL(path != NULL, TINY_RET_E_NULL_PTR);

    file = fopen(path, "a");
    if (file == NULL)
    {
        return TINY_RET_E_IO;
    }

    tiny_log_lock();
    tiny_log_reset();

    g_log_state.file = file;
    g_log_state.owns_file = true;
    g_log_state.console = false;
    g_log_state.level = level;
    if (tag != NULL)
    {
        tiny_snprintf(g_log_state.tag, sizeof(g_log_state.tag), "%s", tag);
    }

#if TINYCORE_HAS_PTHREAD
    if (!g_log_state.mutex_ready)
    {
        if (pthread_mutex_init(&g_log_state.mutex, NULL) == 0)
        {
            g_log_state.mutex_ready = true;
        }
    }
#endif
    tiny_log_unlock();

    return TINY_RET_OK;
}

void tiny_log_close(void)
{
    tiny_log_lock();
    tiny_log_reset();
    tiny_log_unlock();
}

void tiny_log_set_level(TinyLogLevel level)
{
    g_log_state.level = level;
}

TinyLogLevel tiny_log_get_level(void)
{
    return g_log_state.level;
}

static FILE *tiny_log_output(void)
{
    if (g_log_state.file != NULL)
    {
        return g_log_state.file;
    }

    return stderr;
}

void tiny_log_vwrite(TinyLogLevel level,
                     const char *file,
                     int line,
                     const char *format,
                     va_list args)
{
    char message[TINYCORE_LOG_LINE_LEN];
    char stamp[32];
    const char *name = tiny_log_level_name(level);
    const char *color = "";
    const char *reset = "";
    bool use_color = false;
    FILE *output = NULL;

    if (format == NULL)
    {
        return;
    }

    if (level < g_log_state.level || level == TINY_LOG_LEVEL_UNKNOWN)
    {
        return;
    }

    message[0] = '\0';
    tiny_vsnprintf(message, sizeof(message), format, args);

    stamp[0] = '\0';
    if (tiny_getstrtime(stamp, sizeof(stamp)) != TINY_RET_OK)
    {
        tiny_snprintf(stamp, sizeof(stamp), "%s", "0000-00-00 00:00:00");
    }

    tiny_log_lock();

    output = tiny_log_output();
    use_color = g_log_state.console && tiny_log_console_color_supported();
    if (use_color)
    {
        color = tiny_log_level_color(level);
        reset = "\033[0m";
    }

    if (file != NULL && line > 0)
    {
        fprintf(output, "%s %s[%s]%s [%s] %s:%d: %s\n",
                stamp, color, name, reset, g_log_state.tag, file, line, message);
    }
    else
    {
        fprintf(output, "%s %s[%s]%s [%s] %s\n",
                stamp, color, name, reset, g_log_state.tag, message);
    }

    fflush(output);
    tiny_log_unlock();
}

void tiny_log_write(TinyLogLevel level,
                    const char *file,
                    int line,
                    const char *format,
                    ...)
{
    va_list args;

    va_start(args, format);
    tiny_log_vwrite(level, file, line, format, args);
    va_end(args);
}
