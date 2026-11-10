/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_URL_SPLIT_H
#define TINYCORE_TINY_URL_SPLIT_H

#include <tinycore/tiny_ret.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/*
 * Split an "http://host[:port][/uri]" URL.
 *
 *   host  receives a NUL terminated host name or IPv4 address
 *   port  receives the port number, or 80 when the URL omits it
 *   uri   receives the path and query, including the leading '/', or an
 *         empty string when the URL has no path
 *
 * Returns TINY_RET_E_URL_INVALID for malformed URLs and
 * TINY_RET_E_BUFFER_TRUNCATED when host or uri does not fit.
 * IPv6 literals are not supported.
 */
TINYCORE_API TINYCORE_RET tiny_url_split(const char *url,
                                         char host[],
                                         size_t host_size,
                                         uint16_t *port,
                                         char uri[],
                                         size_t uri_size);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_URL_SPLIT_H */
