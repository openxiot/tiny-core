/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_BOOLEAN_H
#define TINYCORE_JSON_BOOLEAN_H

#include <tinycore/json_definitions.h>

TINYCORE_BEGIN_DECLS

TINYCORE_API JsonBoolean *JsonBoolean_New(bool value);
TINYCORE_API void JsonBoolean_Delete(JsonBoolean *boolean);
TINYCORE_API bool JsonBoolean_Equals(const JsonBoolean *boolean, const JsonBoolean *other);
TINYCORE_API bool JsonBoolean_GetValue(const JsonBoolean *boolean);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_BOOLEAN_H */
