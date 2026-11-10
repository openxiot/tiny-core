/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_OBJECT_H
#define TINYCORE_JSON_OBJECT_H

#include <tinycore/buffer.h>
#include <tinycore/json_definitions.h>

TINYCORE_BEGIN_DECLS

TINYCORE_API JsonObject *JsonObject_New(void);
TINYCORE_API JsonObject *JsonObject_NewFrom(const JsonObject *other);

/* Parse a JSON document whose root is an object.  Returns NULL when the
 * document is incomplete or invalid. */
TINYCORE_API JsonObject *JsonObject_NewString(const char *string);

/* Historical alias of JsonObject_NewString(). */
TINYCORE_API JsonObject *JsonObject_NewStringFast(const char *string);

TINYCORE_API void JsonObject_Delete(JsonObject *object);

TINYCORE_API TINYCORE_RET JsonObject_Construct(JsonObject *object);
TINYCORE_API void JsonObject_Dispose(JsonObject *object);

/* Serialize into buffer (which is cleared first). */
TINYCORE_API TINYCORE_RET JsonObject_Encode(const JsonObject *object,
                                            bool pretty,
                                            Buffer *buffer);

/* Serialize into a newly allocated string; release it with tiny_free(). */
TINYCORE_API char *JsonObject_ToString(const JsonObject *object, bool pretty);

TINYCORE_API uint32_t JsonObject_GetSize(const JsonObject *object);
TINYCORE_API bool JsonObject_IsEmpty(const JsonObject *object);
TINYCORE_API bool JsonObject_ContainsKey(const JsonObject *object, const char *key);

TINYCORE_API JsonValue *JsonObject_GetValue(const JsonObject *object, const char *key);
TINYCORE_API JsonObject *JsonObject_GetObject(const JsonObject *object, const char *key);
TINYCORE_API JsonNumber *JsonObject_GetNumber(const JsonObject *object, const char *key);
TINYCORE_API JsonArray *JsonObject_GetArray(const JsonObject *object, const char *key);
TINYCORE_API JsonString *JsonObject_GetString(const JsonObject *object, const char *key);
TINYCORE_API JsonBoolean *JsonObject_GetBoolean(const JsonObject *object, const char *key);

/* Convenience accessor returning the C string of a string member, or NULL. */
TINYCORE_API const char *JsonObject_GetStringValue(const JsonObject *object, const char *key);

/* Put* functions insert or replace and take ownership of their payload.
 * When a value is replaced, the old value is released. */
TINYCORE_API TINYCORE_RET JsonObject_PutValue(JsonObject *object, const char *key, JsonValue *value);
TINYCORE_API TINYCORE_RET JsonObject_PutString(JsonObject *object, const char *key, const char *value);
TINYCORE_API TINYCORE_RET JsonObject_PutInteger(JsonObject *object, const char *key, int32_t value);
TINYCORE_API TINYCORE_RET JsonObject_PutFloat(JsonObject *object, const char *key, double value);
TINYCORE_API TINYCORE_RET JsonObject_PutBoolean(JsonObject *object, const char *key, bool value);
TINYCORE_API TINYCORE_RET JsonObject_PutNull(JsonObject *object, const char *key);
TINYCORE_API TINYCORE_RET JsonObject_PutNumber(JsonObject *object, const char *key, JsonNumber *value);
TINYCORE_API TINYCORE_RET JsonObject_PutObject(JsonObject *object, const char *key, JsonObject *value);
TINYCORE_API TINYCORE_RET JsonObject_PutArray(JsonObject *object, const char *key, JsonArray *value);

/* Insert a deep copy; the caller keeps ownership of value. */
TINYCORE_API TINYCORE_RET JsonObject_AddValue(JsonObject *object, const char *key, const JsonValue *value);

TINYCORE_API TINYCORE_RET JsonObject_Remove(JsonObject *object, const char *key);

TINYCORE_API bool JsonObject_Equals(const JsonObject *object, const JsonObject *other);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_OBJECT_H */
