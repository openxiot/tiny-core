/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_TINY_RET_H
#define TINYCORE_TINY_RET_H

#include <tinycore/tiny_api.h>
#include <tinycore/tiny_typedef.h>

TINYCORE_BEGIN_DECLS

/*
 * TINYCORE_RET is a 32-bit result code:
 *
 *   bit 31      severity     0 = success, 1 = failure
 *   bits 29-30  error level  (reserved, currently always 0)
 *   bits 0-28   error code
 *
 * Successful calls return codes >= 0, failures < 0.  Always test with
 * TINYCORE_RET_SUCCEEDED / TINYCORE_RET_FAILED instead of comparing to 0.
 */
typedef int32_t TINYCORE_RET;

typedef int32_t TinyRet;

#define TINYCORE_RET_CODE_MASK   0x1FFFFFFF
#define TINYCORE_RET_LEVEL_MASK  0x60000000
#define TINYCORE_RET_FAIL_MASK   0x80000000

#define MAKE_RET(code)           ((TINYCORE_RET)(code))
#define TINYCORE_RET_GET_CODE(r) ((int32_t)((r) & TINYCORE_RET_CODE_MASK))

#define TINYCORE_RET_SUCCEEDED(r) ((r) >= 0)
#define TINYCORE_RET_FAILED(r)    ((r) < 0)

/* Compatibility aliases used by older TinyCore releases. */
#define RET_SUCCEEDED(r) TINYCORE_RET_SUCCEEDED(r)
#define RET_FAILED(r)    TINYCORE_RET_FAILED(r)
#define TINY_RET_CODE(r) TINYCORE_RET_GET_CODE(r)

/* Success. */
#define TINY_RET_OK                        MAKE_RET(0)

/* Generic failures. */
#define TINY_RET_E_UNKNOWN                 MAKE_RET(-1)
#define TINY_RET_E_PARAM_INVALID           MAKE_RET(-2)
#define TINY_RET_E_NULL_PTR                MAKE_RET(-3)
#define TINY_RET_E_NOT_INIT                MAKE_RET(-4)
#define TINY_RET_E_NOT_IMPLEMENT           MAKE_RET(-5)
#define TINY_RET_E_NOT_SUPPORT             MAKE_RET(-6)
#define TINY_RET_E_NOT_FOUND               MAKE_RET(-7)
#define TINY_RET_E_OUT_OF_RANGE            MAKE_RET(-8)
#define TINY_RET_E_ACCESS                  MAKE_RET(-9)
#define TINY_RET_E_EXCEED_MAX              MAKE_RET(-10)
#define TINY_RET_E_NOT_EXIST               MAKE_RET(-11)

/* Memory. */
#define TINY_RET_E_NO_MEMORY               MAKE_RET(-12)
#define TINY_RET_E_OUT_OF_MEMORY           MAKE_RET(-13)
#define TINY_RET_E_MALLOC_FAIL             MAKE_RET(-14)

/* Buffer and container. */
#define TINY_RET_E_EMPTY                   MAKE_RET(-15)
#define TINY_RET_E_FULL                    MAKE_RET(-16)
#define TINY_RET_E_NOT_ENOUGH              MAKE_RET(-17)
#define TINY_RET_E_BUFFER_FULL             MAKE_RET(-18)
#define TINY_RET_E_BUFFER_EMPTY            MAKE_RET(-19)
#define TINY_RET_E_BUFFER_OVERFLOW         MAKE_RET(-20)
#define TINY_RET_E_BUFFER_OVERFLOW_LOW     MAKE_RET(-21)
#define TINY_RET_E_BUFFER_TRUNCATED        MAKE_RET(-22)
#define TINY_RET_E_BUFFER_CORRUPTED        MAKE_RET(-23)

/* Parsing. */
#define TINY_RET_E_INVALID_ARGUMENT        MAKE_RET(-24)
#define TINY_RET_E_INVALID_PARAM           MAKE_RET(-25)
#define TINY_RET_E_INVALID_DATA            MAKE_RET(-26)
#define TINY_RET_E_PARSE_FAIL              MAKE_RET(-27)
#define TINY_RET_E_INVALID_FORMAT          MAKE_RET(-28)
#define TINY_RET_E_XML_INVALID             MAKE_RET(-29)

/* Runtime / system. */
#define TINY_RET_E_TIMEOUT                 MAKE_RET(-30)
#define TINY_RET_E_BUSY                    MAKE_RET(-31)
#define TINY_RET_E_INTERRUPTED             MAKE_RET(-32)
#define TINY_RET_E_IO                      MAKE_RET(-33)
#define TINY_RET_E_EXIST                   MAKE_RET(-34)

/* Reserved for library-specific use (JSON, TLV8, ...). */
#define TINY_RET_E_JSON_ERROR              MAKE_RET(-40)
#define TINY_RET_E_TLV8_ERROR              MAKE_RET(-41)
#define TINY_RET_E_URL_INVALID             MAKE_RET(-42)

/* Descriptive names for result codes; never returns NULL. */
TINYCORE_API const char *TinyRet_GetDescription(TINYCORE_RET ret);

TINYCORE_END_DECLS

#endif /* TINYCORE_TINY_RET_H */
