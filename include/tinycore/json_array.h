/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_ARRAY_H
#define TINYCORE_JSON_ARRAY_H

#include <tinycore/json_definitions.h>

TINYCORE_BEGIN_DECLS

TINYCORE_API JsonArray *JsonArray_New(void);
TINYCORE_API JsonArray *JsonArray_NewFrom(const JsonArray *array);
TINYCORE_API void JsonArray_Delete(JsonArray *array);

/*
 * Deep copy src into an already constructed dst.  Any previous content of
 * dst is released.  JsonArray_NewFrom() returns a new array rather than
 * an in-place copy, so this helper bridges the two styles.
 */
TINYCORE_API TINYCORE_RET JsonArray_CopyInto(JsonArray *dst, const JsonArray *src);

TINYCORE_API TINYCORE_RET JsonArray_Construct(JsonArray *array);
TINYCORE_API void JsonArray_Dispose(JsonArray *array);

/* Add* functions take ownership of their payload. */
TINYCORE_API TINYCORE_RET JsonArray_AddValue(JsonArray *array, JsonValue *value);
TINYCORE_API TINYCORE_RET JsonArray_AddString(JsonArray *array, const char *value);
TINYCORE_API TINYCORE_RET JsonArray_AddInteger(JsonArray *array, int32_t value);
TINYCORE_API TINYCORE_RET JsonArray_AddFloat(JsonArray *array, double value);
TINYCORE_API TINYCORE_RET JsonArray_AddBoolean(JsonArray *array, bool value);
TINYCORE_API TINYCORE_RET JsonArray_AddNull(JsonArray *array);
TINYCORE_API TINYCORE_RET JsonArray_AddNumber(JsonArray *array, JsonNumber *value);
TINYCORE_API TINYCORE_RET JsonArray_AddObject(JsonArray *array, JsonObject *value);
TINYCORE_API TINYCORE_RET JsonArray_AddArray(JsonArray *array, JsonArray *value);

TINYCORE_API uint32_t JsonArray_GetSize(const JsonArray *array);
TINYCORE_API bool JsonArray_IsEmpty(const JsonArray *array);

TINYCORE_API JsonValue *JsonArray_GetValue(const JsonArray *array, uint32_t index);
TINYCORE_API JsonString *JsonArray_GetString(const JsonArray *array, uint32_t index);
TINYCORE_API JsonNumber *JsonArray_GetNumber(const JsonArray *array, uint32_t index);
TINYCORE_API JsonObject *JsonArray_GetObject(const JsonArray *array, uint32_t index);
TINYCORE_API JsonArray *JsonArray_GetArray(const JsonArray *array, uint32_t index);
TINYCORE_API JsonBoolean *JsonArray_GetBoolean(const JsonArray *array, uint32_t index);

/* Remove every element. */
TINYCORE_API void JsonArray_Clean(JsonArray *array);

TINYCORE_API bool JsonArray_CheckValuesType(const JsonArray *array, JsonValueType type);
TINYCORE_API bool JsonArray_Equals(const JsonArray *array, const JsonArray *other);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_ARRAY_H */
