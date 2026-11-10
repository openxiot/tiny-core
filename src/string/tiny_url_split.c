/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_url_split.h>
#include <tinycore/tiny_char_util.h>

#define TINY_URL_SCHEME "http://"
#define TINY_URL_DEFAULT_PORT 80U

static const char *tiny_url_skip_scheme(const char *url)
{
    size_t i = 0;

    for (i = 0; TINY_URL_SCHEME[i] != '\0'; ++i)
    {
        char c = url[i];

        if (c >= 'A' && c <= 'Z')
        {
            c = (char)(c - 'A' + 'a');
        }

        if (c != TINY_URL_SCHEME[i])
        {
            return NULL;
        }
    }

    return url + i;
}

TINYCORE_RET tiny_url_split(const char *url,
                            char host[],
                            size_t host_size,
                            uint16_t *port,
                            char uri[],
                            size_t uri_size)
{
    const char *cursor = NULL;
    const char *host_start = NULL;
    size_t host_length = 0;
    size_t uri_length = 0;
    unsigned long port_value = TINY_URL_DEFAULT_PORT;

    if (url == NULL || host == NULL || port == NULL || uri == NULL)
    {
        return TINY_RET_E_NULL_PTR;
    }

    if (host_size == 0 || uri_size == 0)
    {
        return TINY_RET_E_PARAM_INVALID;
    }

    cursor = tiny_url_skip_scheme(url);
    if (cursor == NULL)
    {
        return TINY_RET_E_URL_INVALID;
    }

    host_start = cursor;
    while (*cursor != '\0' && *cursor != ':' && *cursor != '/')
    {
        ++cursor;
    }

    host_length = (size_t)(cursor - host_start);
    if (host_length == 0)
    {
        return TINY_RET_E_URL_INVALID;
    }

    if (host_length >= host_size)
    {
        return TINY_RET_E_BUFFER_TRUNCATED;
    }

    memcpy(host, host_start, host_length);
    host[host_length] = '\0';

    if (*cursor == ':')
    {
        const char *port_start = ++cursor;

        port_value = 0;

        while (tiny_is_digit(*cursor))
        {
            port_value = port_value * 10U + (unsigned long)(*cursor - '0');
            if (port_value > 65535U)
            {
                return TINY_RET_E_URL_INVALID;
            }
            ++cursor;
        }

        if (cursor == port_start || port_value == 0U)
        {
            return TINY_RET_E_URL_INVALID;
        }

        if (*cursor != '\0' && *cursor != '/')
        {
            return TINY_RET_E_URL_INVALID;
        }
    }

    *port = (uint16_t)port_value;

    if (*cursor == '/')
    {
        uri_length = strlen(cursor);

        if (uri_length >= uri_size)
        {
            return TINY_RET_E_BUFFER_TRUNCATED;
        }

        memcpy(uri, cursor, uri_length);
    }
    else
    {
        uri_length = 0;
    }

    uri[uri_length] = '\0';

    return TINY_RET_OK;
}
