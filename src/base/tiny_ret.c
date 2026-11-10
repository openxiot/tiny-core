/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_ret.h>

typedef struct TinyRetDescription
{
    TINYCORE_RET ret;
    const char *description;
} TinyRetDescription;

static const TinyRetDescription kTinyRetDescriptions[] =
{
    { TINY_RET_OK,                    "ok" },
    { TINY_RET_E_UNKNOWN,             "unknown error" },
    { TINY_RET_E_PARAM_INVALID,       "invalid parameter" },
    { TINY_RET_E_NULL_PTR,            "null pointer" },
    { TINY_RET_E_NOT_INIT,            "not initialized" },
    { TINY_RET_E_NOT_IMPLEMENT,       "not implemented" },
    { TINY_RET_E_NOT_SUPPORT,         "not supported" },
    { TINY_RET_E_NOT_FOUND,           "not found" },
    { TINY_RET_E_OUT_OF_RANGE,        "out of range" },
    { TINY_RET_E_ACCESS,              "access denied" },
    { TINY_RET_E_EXCEED_MAX,          "exceeded maximum" },
    { TINY_RET_E_NOT_EXIST,           "does not exist" },
    { TINY_RET_E_NO_MEMORY,           "no memory" },
    { TINY_RET_E_OUT_OF_MEMORY,       "out of memory" },
    { TINY_RET_E_MALLOC_FAIL,         "allocation failed" },
    { TINY_RET_E_EMPTY,               "empty" },
    { TINY_RET_E_FULL,                "full" },
    { TINY_RET_E_NOT_ENOUGH,          "not enough" },
    { TINY_RET_E_BUFFER_FULL,         "buffer full" },
    { TINY_RET_E_BUFFER_EMPTY,        "buffer empty" },
    { TINY_RET_E_BUFFER_OVERFLOW,     "buffer overflow" },
    { TINY_RET_E_BUFFER_OVERFLOW_LOW, "buffer overflow (low level)" },
    { TINY_RET_E_BUFFER_TRUNCATED,    "buffer truncated" },
    { TINY_RET_E_BUFFER_CORRUPTED,    "buffer corrupted" },
    { TINY_RET_E_INVALID_ARGUMENT,    "invalid argument" },
    { TINY_RET_E_INVALID_PARAM,       "invalid parameter value" },
    { TINY_RET_E_INVALID_DATA,        "invalid data" },
    { TINY_RET_E_PARSE_FAIL,          "parse failed" },
    { TINY_RET_E_INVALID_FORMAT,      "invalid format" },
    { TINY_RET_E_XML_INVALID,         "invalid XML" },
    { TINY_RET_E_TIMEOUT,             "timeout" },
    { TINY_RET_E_BUSY,                "busy" },
    { TINY_RET_E_INTERRUPTED,         "interrupted" },
    { TINY_RET_E_IO,                  "I/O error" },
    { TINY_RET_E_EXIST,               "already exists" },
    { TINY_RET_E_JSON_ERROR,          "JSON error" },
    { TINY_RET_E_TLV8_ERROR,          "TLV8 error" },
    { TINY_RET_E_URL_INVALID,         "invalid URL" }
};

const char *TinyRet_GetDescription(TINYCORE_RET ret)
{
    size_t i = 0;

    for (i = 0; i < sizeof(kTinyRetDescriptions) / sizeof(kTinyRetDescriptions[0]); ++i)
    {
        if (kTinyRetDescriptions[i].ret == ret)
        {
            return kTinyRetDescriptions[i].description;
        }
    }

    return "unknown error";
}
