/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_STRING_H
#define TINYCORE_JSON_STRING_H

#include <tinycore/json_definitions.h>

TINYCORE_BEGIN_DECLS

TINYCORE_API JsonString *JsonString_New(const char *value);

/* Copy exactly length bytes; the result is NUL terminated.  length may
 * cover embedded NUL bytes. */
TINYCORE_API JsonString *JsonString_NewLength(const char *value, size_t length);

TINYCORE_API void JsonString_Delete(JsonString *string);

TINYCORE_API bool JsonString_Equals(const JsonString *string, const JsonString *other);

TINYCORE_API const char *JsonString_GetValue(const JsonString *string);
TINYCORE_API size_t JsonString_GetLength(const JsonString *string);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_STRING_H */
