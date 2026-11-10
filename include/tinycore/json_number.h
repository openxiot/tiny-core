/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_NUMBER_H
#define TINYCORE_JSON_NUMBER_H

#include <tinycore/json_definitions.h>

TINYCORE_BEGIN_DECLS

TINYCORE_API JsonNumber *JsonNumber_NewInteger(int32_t value);
TINYCORE_API JsonNumber *JsonNumber_NewFloat(double value);
TINYCORE_API void JsonNumber_Delete(JsonNumber *number);

TINYCORE_API bool JsonNumber_Equals(const JsonNumber *number, const JsonNumber *other);

TINYCORE_API bool JsonNumber_IsInteger(const JsonNumber *number);
TINYCORE_API int32_t JsonNumber_GetInteger(const JsonNumber *number);
TINYCORE_API double JsonNumber_GetFloat(const JsonNumber *number);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_NUMBER_H */
