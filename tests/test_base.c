/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tinycore/tiny_alloc.h>
#include <tinycore/tiny_log.h>
#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_snprintf.h>
#include <tinycore/tiny_strdup.h>
#include <tinycore/tiny_time.h>

#define LOG_PATH "tinycore_base_log_9f3b.tmp"

static int format_with_arguments(char *buffer, size_t size, const char *format, ...)
{
    va_list args;
    int result = 0;

    va_start(args, format);
    result = tiny_vsnprintf(buffer, size, format, args);
    va_end(args);

    return result;
}

static void test_ret_descriptions(void)
{
    TinyRet ret = MAKE_RET(-7);

    TINY_TEST_CHECK(strcmp(TinyRet_GetDescription(TINY_RET_OK), "ok") == 0);
    TINY_TEST_CHECK(strcmp(TinyRet_GetDescription(TINY_RET_E_NULL_PTR), "null pointer") == 0);
    TINY_TEST_CHECK(strcmp(TinyRet_GetDescription(TINY_RET_E_IO), "I/O error") == 0);
    TINY_TEST_CHECK(strcmp(TinyRet_GetDescription(TINY_RET_E_URL_INVALID), "invalid URL") == 0);
    TINY_TEST_CHECK(strcmp(TinyRet_GetDescription(MAKE_RET(-9999)), "unknown error") == 0);
    TINY_TEST_CHECK(TinyRet_GetDescription(MAKE_RET(-9999)) != NULL);

    TINY_TEST_CHECK(ret == TINY_RET_E_NOT_FOUND);
    TINY_TEST_CHECK(TINY_RET_SUCCEEDED(TINY_RET_OK));
    TINY_TEST_CHECK(!TINY_RET_SUCCEEDED(TINY_RET_E_UNKNOWN));
    TINY_TEST_CHECK(TINY_RET_FAILED(TINY_RET_E_UNKNOWN));
    TINY_TEST_CHECK(RET_SUCCEEDED(TINY_RET_OK));
    TINY_TEST_CHECK(RET_FAILED(TINY_RET_E_UNKNOWN));
    TINY_TEST_CHECK(TINY_RET_CODE(TINY_RET_E_NOT_FOUND) ==
                    (int32_t)(TINY_RET_E_NOT_FOUND & TINY_RET_CODE_MASK));
}

static void test_alloc(void)
{
    uint8_t *zeros = (uint8_t *)tiny_calloc(16, sizeof(uint8_t));
    uint8_t *single = (uint8_t *)tiny_malloc(0);
    int *values = (int *)tiny_calloc(0, 0);
    int *grown = NULL;
    size_t i = 0;

    TINY_TEST_CHECK(zeros != NULL);
    if (zeros != NULL)
    {
        for (i = 0; i < 16; ++i)
        {
            TINY_TEST_CHECK(zeros[i] == 0);
        }
    }
    tiny_free(zeros);

    /* Zero sized requests still return a usable block. */
    TINY_TEST_CHECK(single != NULL);
    tiny_free(single);
    TINY_TEST_CHECK(values != NULL);
    tiny_free(values);

    values = (int *)tiny_malloc(4U * sizeof(int));
    TINY_TEST_CHECK(values != NULL);
    if (values == NULL)
    {
        return;
    }

    values[0] = 11;
    values[1] = 22;
    values[2] = 33;
    values[3] = 44;

    grown = (int *)tiny_realloc(values, 64U * sizeof(int));
    TINY_TEST_CHECK(grown != NULL);
    if (grown != NULL)
    {
        TINY_TEST_CHECK(grown[0] == 11 && grown[1] == 22);
        TINY_TEST_CHECK(grown[2] == 33 && grown[3] == 44);
        values = grown;
    }

    /* realloc(ptr, 0) frees the block and returns NULL. */
    TINY_TEST_CHECK(tiny_realloc(values, 0) == NULL);

    /* realloc(NULL, n) behaves like malloc. */
    grown = (int *)tiny_realloc(NULL, 2U * sizeof(int));
    TINY_TEST_CHECK(grown != NULL);
    tiny_free(grown);

    tiny_free(NULL); /* no-op */
}

static void test_strdup(void)
{
    static const char literal[] = "tinycore";
    char *copy = tiny_strdup(literal);
    char *short_copy = tiny_strndup("abcdef", 3);
    char *long_copy = tiny_strndup("abc", 10);
    char *empty_copy = tiny_strndup("abc", 0);

    TINY_TEST_CHECK(tiny_strdup(NULL) == NULL);
    TINY_TEST_CHECK(tiny_strndup(NULL, 4) == NULL);

    TINY_TEST_CHECK(copy != NULL);
    if (copy != NULL)
    {
        TINY_TEST_CHECK(strcmp(copy, "tinycore") == 0);
        TINY_TEST_CHECK(copy != literal);
        copy[0] = 'T';
        TINY_TEST_CHECK(strcmp(copy, "Tinycore") == 0);
    }

    TINY_TEST_CHECK(short_copy != NULL);
    TINY_TEST_CHECK(short_copy != NULL && strcmp(short_copy, "abc") == 0);

    TINY_TEST_CHECK(long_copy != NULL);
    TINY_TEST_CHECK(long_copy != NULL && strcmp(long_copy, "abc") == 0);

    TINY_TEST_CHECK(empty_copy != NULL);
    TINY_TEST_CHECK(empty_copy != NULL && strcmp(empty_copy, "") == 0);

    tiny_free(copy);
    tiny_free(short_copy);
    tiny_free(long_copy);
    tiny_free(empty_copy);
}

static void test_snprintf(void)
{
    char buffer[32];
    int result = 0;

    result = tiny_snprintf(buffer, sizeof(buffer), "%d-%s", 7, "x");
    TINY_TEST_CHECK(result == 3);
    TINY_TEST_CHECK(strcmp(buffer, "7-x") == 0);

    result = format_with_arguments(buffer, sizeof(buffer), "%s/%u", "id", 42U);
    TINY_TEST_CHECK(result == 5);
    TINY_TEST_CHECK(strcmp(buffer, "id/42") == 0);

    memset(buffer, 'z', sizeof(buffer));
    result = tiny_snprintf(buffer, 4, "abcdef");
    TINY_TEST_CHECK(result == 6); /* required length */
    TINY_TEST_CHECK(strcmp(buffer, "abc") == 0);

    result = tiny_snprintf(buffer, sizeof(buffer), "%s", "");
    TINY_TEST_CHECK(result == 0);
    TINY_TEST_CHECK(buffer[0] == '\0');
}

static void test_time(void)
{
    uint64_t first = tiny_current_microsecond();
    uint64_t milliseconds = tiny_current_millisecond();
    uint64_t second = tiny_current_microsecond();
    char buffer[64];
    uint64_t before = 0;
    uint64_t after = 0;

    TINY_TEST_CHECK(first > 0);
    TINY_TEST_CHECK(second >= first);
    TINY_TEST_CHECK(milliseconds > 0);

    /* millisecond() is derived from microsecond(), so it must agree within
     * the time spent between the two calls. */
    TINY_TEST_CHECK(milliseconds * 1000ULL >= first - 1000ULL);
    TINY_TEST_CHECK(milliseconds * 1000ULL <= second + 1000ULL);

    TINY_TEST_CHECK(tiny_getstrtime(buffer, sizeof(buffer)) == TINY_RET_OK);
    TINY_TEST_CHECK(strlen(buffer) == 19);
    TINY_TEST_CHECK(buffer[4] == '-' && buffer[7] == '-' && buffer[10] == ' ');
    TINY_TEST_CHECK(buffer[13] == ':' && buffer[16] == ':');

    TINY_TEST_CHECK(tiny_getstrtime_format(buffer, sizeof(buffer), "literal") == TINY_RET_OK);
    TINY_TEST_CHECK(strcmp(buffer, "literal") == 0);

    /* Output that cannot fit reports NOT_ENOUGH and leaves the buffer empty. */
    buffer[0] = 'x';
    TINY_TEST_CHECK(tiny_getstrtime_format(buffer, 2, "%Y") == TINY_RET_E_NOT_ENOUGH);
    TINY_TEST_CHECK(buffer[0] == '\0');

    TINY_TEST_CHECK(tiny_getstrtime(NULL, 8) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(tiny_getstrtime(buffer, 0) == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(tiny_getstrtime_format(buffer, sizeof(buffer), NULL) == TINY_RET_E_NULL_PTR);

    /* Sleeping for zero returns immediately; a real sleep does not return
     * early. */
    tiny_sleep_ms(0);
    tiny_sleep_us(0);

    before = tiny_current_microsecond();
    tiny_sleep_ms(5);
    after = tiny_current_microsecond();
    TINY_TEST_CHECK(after - before >= 1000ULL);
}

static size_t read_text_file(const char *path, char *buffer, size_t size)
{
    FILE *file = fopen(path, "rb");
    size_t count = 0;

    if (file == NULL)
    {
        return 0;
    }

    count = fread(buffer, 1U, size - 1U, file);
    fclose(file);
    buffer[count] = '\0';

    return count;
}

static void log_with_arguments(TinyLogLevel level,
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

static void test_log(void)
{
    const char *null_format = NULL;
    char content[8192];

    tiny_log_close();
    remove(LOG_PATH);

    TINY_TEST_CHECK(tiny_log_init_file("base", TINY_LOG_LEVEL_INFO, LOG_PATH) == TINY_RET_OK);
    TINY_TEST_CHECK(tiny_log_get_level() == TINY_LOG_LEVEL_INFO);

    tiny_log_write(TINY_LOG_LEVEL_INFO, "base.c", 42, "hello %d", 7);
    log_with_arguments(TINY_LOG_LEVEL_INFO, "base.c", 46, "vwrite %s", "value");
    tiny_log_write(TINY_LOG_LEVEL_DEBUG, "base.c", 43, "hidden debug");
    tiny_log_write(TINY_LOG_LEVEL_UNKNOWN, "base.c", 44, "hidden unknown");
    tiny_log_write(TINY_LOG_LEVEL_INFO, NULL, 0, "no location");
    {
        /* Call through a pointer so the compiler's format checking does not
         * reject the intentionally NULL format; the call must be ignored. */
        void (*write_log)(TinyLogLevel, const char *, int, const char *, ...) = tiny_log_write;

        write_log(TINY_LOG_LEVEL_INFO, "base.c", 45, null_format);
    }

    /* The binary dump goes through the same log level. */
    tiny_print_binary("A\n", 2);
    tiny_log_close();

    TINY_TEST_CHECK(read_text_file(LOG_PATH, content, sizeof(content)) > 0);
    TINY_TEST_CHECK(strstr(content, "hello 7") != NULL);
    TINY_TEST_CHECK(strstr(content, "vwrite value") != NULL);
    TINY_TEST_CHECK(strstr(content, "base.c:46:") != NULL);
    TINY_TEST_CHECK(strstr(content, "[INFO]") != NULL);
    TINY_TEST_CHECK(strstr(content, "[base]") != NULL);
    TINY_TEST_CHECK(strstr(content, "base.c:42:") != NULL);
    TINY_TEST_CHECK(strstr(content, "hidden") == NULL);
    TINY_TEST_CHECK(strstr(content, "no location") != NULL);
    TINY_TEST_CHECK(strstr(content, "41 0a") != NULL);
    TINY_TEST_CHECK(strstr(content, "|A.|") != NULL);

    /* Reopening appends instead of truncating. */
    TINY_TEST_CHECK(tiny_log_init_file("base", TINY_LOG_LEVEL_WARN, LOG_PATH) == TINY_RET_OK);
    tiny_log_set_level(TINY_LOG_LEVEL_WARN);
    TINY_TEST_CHECK(tiny_log_get_level() == TINY_LOG_LEVEL_WARN);
    tiny_log_write(TINY_LOG_LEVEL_WARN, NULL, 0, "second run");
    tiny_log_write(TINY_LOG_LEVEL_INFO, NULL, 0, "filtered info");
    tiny_log_close();

    TINY_TEST_CHECK(read_text_file(LOG_PATH, content, sizeof(content)) > 0);
    TINY_TEST_CHECK(strstr(content, "hello 7") != NULL);
    TINY_TEST_CHECK(strstr(content, "second run") != NULL);
    TINY_TEST_CHECK(strstr(content, "filtered info") == NULL);

    /* Console mode and argument errors. */
    TINY_TEST_CHECK(tiny_log_init(NULL, TINY_LOG_LEVEL_WARN) == TINY_RET_OK);
    tiny_log_set_level(TINY_LOG_LEVEL_ERROR);
    TINY_TEST_CHECK(tiny_log_get_level() == TINY_LOG_LEVEL_ERROR);
    tiny_log_close();

    TINY_TEST_CHECK(tiny_log_init_file("base", TINY_LOG_LEVEL_INFO, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(tiny_log_init_file("base", TINY_LOG_LEVEL_INFO,
                                       "tinycore_no_such_dir_9f3b/file.log") == TINY_RET_E_IO);

    tiny_print_binary(NULL, 0); /* no-op */
    tiny_log_close();
    remove(LOG_PATH);
}

int main(void)
{
    TINY_TEST_BEGIN("base");
    test_ret_descriptions();
    test_alloc();
    test_strdup();
    test_snprintf();
    test_time();
    test_log();
    return TINY_TEST_FINISH();
}
