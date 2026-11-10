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

#include <tinycore/string_array.h>
#include <tinycore/tiny_buffer_append.h>
#include <tinycore/tiny_char_util.h>
#include <tinycore/tiny_str_equal.h>
#include <tinycore/tiny_str_split.h>
#include <tinycore/tiny_strnlen.h>
#include <tinycore/tiny_url_split.h>

static void test_char_util(void)
{
    TINY_TEST_CHECK(tiny_is_ascii('a'));
    TINY_TEST_CHECK(tiny_is_ascii('\0'));
    TINY_TEST_CHECK(!tiny_is_ascii((char)0x80));

    TINY_TEST_CHECK(tiny_is_control('\n'));
    TINY_TEST_CHECK(!tiny_is_control('a'));

    TINY_TEST_CHECK(tiny_is_digit('0'));
    TINY_TEST_CHECK(tiny_is_digit('9'));
    TINY_TEST_CHECK(!tiny_is_digit('a'));

    TINY_TEST_CHECK(tiny_is_hex('0'));
    TINY_TEST_CHECK(tiny_is_hex('F'));
    TINY_TEST_CHECK(tiny_is_hex('a'));
    TINY_TEST_CHECK(!tiny_is_hex('g'));

    TINY_TEST_CHECK(tiny_is_tspecial('('));
    TINY_TEST_CHECK(tiny_is_tspecial('@'));
    TINY_TEST_CHECK(!tiny_is_tspecial('a'));
}

static void test_str_equal(void)
{
    TINY_TEST_CHECK(tiny_str_equal("abc", "abc", false));
    TINY_TEST_CHECK(!tiny_str_equal("abc", "abd", false));
    TINY_TEST_CHECK(tiny_str_equal("AbC", "aBc", true));
    TINY_TEST_CHECK(!tiny_str_equal("abc", "abd", true));
    TINY_TEST_CHECK(!tiny_str_equal("ab", "abc", false));
    TINY_TEST_CHECK(!tiny_str_equal("abc", "ab", false));
    TINY_TEST_CHECK(tiny_str_equal("", "", false));
    TINY_TEST_CHECK(tiny_str_equal(NULL, NULL, false));
    TINY_TEST_CHECK(!tiny_str_equal(NULL, "", false));
    TINY_TEST_CHECK(!tiny_str_equal("", NULL, true));
}

static void test_str_split(void)
{
    char groups[5][TINYCORE_STR_SPLIT_GROUP_SIZE];
    char long_field[TINYCORE_STR_SPLIT_GROUP_SIZE + 16];
    uint32_t count = 0;
    size_t i = 0;

    count = tiny_str_split("AABBCC112233-DDEEFF445566-n32g45x", "-", groups, 5);
    TINY_TEST_CHECK(count == 3);
    TINY_TEST_CHECK(strcmp(groups[0], "AABBCC112233") == 0);
    TINY_TEST_CHECK(strcmp(groups[1], "DDEEFF445566") == 0);
    TINY_TEST_CHECK(strcmp(groups[2], "n32g45x") == 0);

    count = tiny_str_split("a--b", "-", groups, 5);
    TINY_TEST_CHECK(count == 3);
    TINY_TEST_CHECK(strcmp(groups[0], "a") == 0);
    TINY_TEST_CHECK(strcmp(groups[1], "") == 0);
    TINY_TEST_CHECK(strcmp(groups[2], "b") == 0);

    count = tiny_str_split("a,b,c", "", groups, 5);
    TINY_TEST_CHECK(count == 0);

    count = tiny_str_split(NULL, ",", groups, 5);
    TINY_TEST_CHECK(count == 0);

    count = tiny_str_split("a,b,c", ",", groups, 2);
    TINY_TEST_CHECK(count == 2);
    TINY_TEST_CHECK(strcmp(groups[0], "a") == 0);
    TINY_TEST_CHECK(strcmp(groups[1], "b") == 0);

    memset(long_field, 'x', sizeof(long_field) - 1);
    long_field[sizeof(long_field) - 1] = '\0';
    count = tiny_str_split(long_field, ",", groups, 5);
    TINY_TEST_CHECK(count == 1);
    TINY_TEST_CHECK(strlen(groups[0]) == TINYCORE_STR_SPLIT_GROUP_SIZE - 1);

    for (i = 0; i < sizeof(groups[0]); ++i)
    {
        TINY_TEST_CHECK(groups[0][i] != 0 || i == TINYCORE_STR_SPLIT_GROUP_SIZE - 1);
    }
}

static void test_strnlen(void)
{
    TINY_TEST_CHECK(tiny_strnlen("abc", 10) == 3);
    TINY_TEST_CHECK(tiny_strnlen("abcdef", 3) == 3);
    TINY_TEST_CHECK(tiny_strnlen("abc", 3) == 3);
    TINY_TEST_CHECK(tiny_strnlen("", 10) == 0);
    TINY_TEST_CHECK(tiny_strnlen(NULL, 10) == 0);
}

static void test_url_split(void)
{
    char host[64];
    char uri[128];
    uint16_t port = 0;

    TINY_TEST_CHECK(tiny_url_split("http://10.0.1.1:8080/1.mp4", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_OK);
    TINY_TEST_CHECK(strcmp(host, "10.0.1.1") == 0);
    TINY_TEST_CHECK(port == 8080);
    TINY_TEST_CHECK(strcmp(uri, "/1.mp4") == 0);

    TINY_TEST_CHECK(tiny_url_split("HTTP://example.com", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_OK);
    TINY_TEST_CHECK(strcmp(host, "example.com") == 0);
    TINY_TEST_CHECK(port == 80);
    TINY_TEST_CHECK(strcmp(uri, "") == 0);

    TINY_TEST_CHECK(tiny_url_split("http://example.com/a?b=c", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_OK);
    TINY_TEST_CHECK(strcmp(uri, "/a?b=c") == 0);

    TINY_TEST_CHECK(tiny_url_split("http://host:abc/", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_E_URL_INVALID);
    TINY_TEST_CHECK(tiny_url_split("https://host/", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_E_URL_INVALID);
    TINY_TEST_CHECK(tiny_url_split("http:///path", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_E_URL_INVALID);
    TINY_TEST_CHECK(tiny_url_split("http://host:0/", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_E_URL_INVALID);
    TINY_TEST_CHECK(tiny_url_split("http://host:65536/", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_E_URL_INVALID);
    TINY_TEST_CHECK(tiny_url_split("http://host:80x/", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_E_URL_INVALID);
    TINY_TEST_CHECK(tiny_url_split(NULL, host, sizeof(host), &port, uri,
                                   sizeof(uri)) == TINY_RET_E_NULL_PTR);
}

static void test_buffer_append(void)
{
    char text[8];
    size_t offset = 0;

    memset(text, 0, sizeof(text));
    offset += tiny_buffer_append_string(text, sizeof(text), offset, "ab");
    TINY_TEST_CHECK(offset == 2);
    offset += tiny_buffer_append_string(text, sizeof(text), offset, "cd");
    TINY_TEST_CHECK(offset == 4);
    TINY_TEST_CHECK(memcmp(text, "abcd", 4) == 0);
    TINY_TEST_CHECK(text[4] == '\0'); /* caller owns termination */

    TINY_TEST_CHECK(tiny_buffer_append_byte((uint8_t *)text, sizeof(text), offset, 'e') == 1);
    TINY_TEST_CHECK(text[4] == 'e');
    TINY_TEST_CHECK(tiny_buffer_append_byte((uint8_t *)text, 5, 5, 'f') == 0);

    TINY_TEST_CHECK(tiny_buffer_append_bytes((uint8_t *)text, sizeof(text), 4, "ef", 2) == 2);
    TINY_TEST_CHECK(memcmp(text + 4, "ef", 2) == 0);
    TINY_TEST_CHECK(tiny_buffer_append_bytes((uint8_t *)text, 5, 4, "x", 2) == 0);
    TINY_TEST_CHECK(tiny_buffer_append_string(text, 8, 8, "x") == 0);
    TINY_TEST_CHECK(tiny_buffer_append_string(text, 8, 4, "x") == 1);
}

static void test_string_array(void)
{
    StringArray *array = StringArray_NewString("a,,b", ",");

    TINY_TEST_CHECK(array != NULL);
    if (array != NULL)
    {
        TINY_TEST_CHECK(StringArray_GetSize(array) == 3);
        TINY_TEST_CHECK(strcmp(StringArray_GetAt(array, 0), "a") == 0);
        TINY_TEST_CHECK(strcmp(StringArray_GetAt(array, 1), "") == 0);
        TINY_TEST_CHECK(strcmp(StringArray_GetAt(array, 2), "b") == 0);
        TINY_TEST_CHECK(StringArray_GetAt(array, 3) == NULL);
        TINY_TEST_CHECK(StringArray_Add(array, "c") != NULL);
        TINY_TEST_CHECK(StringArray_GetSize(array) == 4);
        TINY_TEST_CHECK(strcmp(StringArray_GetAt(array, 3), "c") == 0);
        StringArray_Delete(array);
    }

    TINY_TEST_CHECK(StringArray_NewString("a", "") == NULL);
    TINY_TEST_CHECK(StringArray_NewString(NULL, ",") == NULL);

    {
        StringArray stack;
        TINY_TEST_CHECK(StringArray_Construct(&stack) == TINY_RET_OK);
        TINY_TEST_CHECK(StringArray_GetSize(&stack) == 0);
        StringArray_Dispose(&stack);
    }
}

static void test_url_split_edges(void)
{
    char host[64];
    char small_host[4];
    char uri[64];
    char small_uri[4];
    uint16_t port = 0;

    TINY_TEST_CHECK(tiny_url_split("http://example.com/", small_host, sizeof(small_host),
                                   &port, uri, sizeof(uri)) == TINY_RET_E_BUFFER_TRUNCATED);
    TINY_TEST_CHECK(tiny_url_split("http://h/abcdef", host, sizeof(host),
                                   &port, small_uri, sizeof(small_uri)) == TINY_RET_E_BUFFER_TRUNCATED);

    TINY_TEST_CHECK(tiny_url_split("http://h:65535/", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_OK);
    TINY_TEST_CHECK(port == 65535);
    TINY_TEST_CHECK(strcmp(uri, "/") == 0);

    TINY_TEST_CHECK(tiny_url_split("http://h", host, sizeof(host),
                                   &port, uri, sizeof(uri)) == TINY_RET_OK);
    TINY_TEST_CHECK(strcmp(host, "h") == 0);
    TINY_TEST_CHECK(port == 80);
    TINY_TEST_CHECK(strcmp(uri, "") == 0);

    TINY_TEST_CHECK(tiny_url_split("ftp://h/", host, sizeof(host), &port, uri,
                                   sizeof(uri)) == TINY_RET_E_URL_INVALID);
    TINY_TEST_CHECK(tiny_url_split("http://h/", host, 0, &port, uri,
                                   sizeof(uri)) == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(tiny_url_split("http://h/", host, sizeof(host), &port, uri,
                                   0) == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(tiny_url_split("http://h/", NULL, sizeof(host), &port, uri,
                                   sizeof(uri)) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(tiny_url_split("http://h/", host, sizeof(host), NULL, uri,
                                   sizeof(uri)) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(tiny_url_split("http://h/", host, sizeof(host), &port, NULL,
                                   sizeof(uri)) == TINY_RET_E_NULL_PTR);
}

static void test_string_array_edges(void)
{
    StringArray *empty = StringArray_New();
    StringArray *split = StringArray_NewString("", ",");
    StringArray stack;

    TINY_TEST_CHECK(StringArray_Construct(NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(StringArray_GetSize(NULL) == 0);
    TINY_TEST_CHECK(StringArray_GetAt(NULL, 0) == NULL);
    StringArray_Dispose(NULL);
    StringArray_Delete(NULL);

    TINY_TEST_CHECK(empty != NULL);
    if (empty != NULL)
    {
        TINY_TEST_CHECK(StringArray_GetSize(empty) == 0);
        TINY_TEST_CHECK(StringArray_GetAt(empty, 0) == NULL);
        TINY_TEST_CHECK(StringArray_Add(empty, NULL) == NULL);
        TINY_TEST_CHECK(StringArray_Add(empty, "one") != NULL);
        TINY_TEST_CHECK(StringArray_Add(empty, "two") != NULL);
        TINY_TEST_CHECK(StringArray_GetSize(empty) == 2);
        TINY_TEST_CHECK(strcmp(StringArray_GetAt(empty, 1), "two") == 0);
        StringArray_Delete(empty);
    }

    /* An empty string still yields one (empty) field. */
    TINY_TEST_CHECK(split != NULL);
    if (split != NULL)
    {
        TINY_TEST_CHECK(StringArray_GetSize(split) == 1);
        TINY_TEST_CHECK(strcmp(StringArray_GetAt(split, 0), "") == 0);
        StringArray_Delete(split);
    }

    /* Dispose releases the owned strings. */
    TINY_TEST_CHECK(StringArray_Construct(&stack) == TINY_RET_OK);
    TINY_TEST_CHECK(StringArray_Add(&stack, "a") != NULL);
    TINY_TEST_CHECK(StringArray_Add(&stack, "b") != NULL);
    StringArray_Dispose(&stack);
    TINY_TEST_CHECK(StringArray_GetSize(&stack) == 0);
}

int main(void)
{
    TINY_TEST_BEGIN("string");
    test_char_util();
    test_str_equal();
    test_str_split();
    test_strnlen();
    test_url_split();
    test_url_split_edges();
    test_buffer_append();
    test_string_array();
    test_string_array_edges();
    return TINY_TEST_FINISH();
}
