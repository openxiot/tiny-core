/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_VALUE_H
#define TINYCORE_JSON_VALUE_H

#include <tinycore/json_definitions.h>

TINYCORE_BEGIN_DECLS

/* All constructors below take ownership of the payload they are given. */

TINYCORE_API JsonValue *JsonValue_New(void); /* JSON_UNDEFINED */
TINYCORE_API JsonValue *JsonValue_NewString(const char *value);
TINYCORE_API JsonValue *JsonValue_NewStringLength(const char *value, size_t length);
TINYCORE_API JsonValue *JsonValue_NewInteger(int32_t value);
TINYCORE_API JsonValue *JsonValue_NewFloat(double value);
TINYCORE_API JsonValue *JsonValue_NewBoolean(bool value);
TINYCORE_API JsonValue *JsonValue_NewNull(void);
TINYCORE_API JsonValue *JsonValue_NewNumber(JsonNumber *number);
TINYCORE_API JsonValue *JsonValue_NewObject(JsonObject *object);
TINYCORE_API JsonValue *JsonValue_NewArray(JsonArray *array);

/* Generic constructor.  For JSON_NULL, value is ignored.  Returns NULL when
 * value is required but NULL. */
TINYCORE_API JsonValue *JsonValue_NewValue(JsonValueType type, void *value);

/* Deep copy. */
TINYCORE_API JsonValue *JsonValue_NewFrom(const JsonValue *other);

TINYCORE_API void JsonValue_Delete(JsonValue *value);

TINYCORE_API bool JsonValue_Equals(const JsonValue *value, const JsonValue *other);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_VALUE_H */
