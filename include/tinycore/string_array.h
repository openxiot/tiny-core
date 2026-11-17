/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_STRING_ARRAY_H
#define TINYCORE_STRING_ARRAY_H

#include <tinycore/tiny_list.h>

TINYCORE_BEGIN_DECLS

/* Owned array of C strings backed by TinyList.  The array owns its strings;
 * they are released when the array is disposed. */
typedef struct
{
    TinyList values; /* char* */
} StringArray;

TINYCORE_API TinyRet StringArray_Construct(StringArray *array);
TINYCORE_API void StringArray_Dispose(StringArray *array);
TINYCORE_API StringArray *StringArray_New(void);
TINYCORE_API void StringArray_Delete(StringArray *array);

/* Split string on separator into an owned array.  Empty fields are kept.
 * Returns NULL when string or separator is NULL or empty, or on allocation
 * failure. */
TINYCORE_API StringArray *StringArray_NewString(const char *string, const char *separator);

/* Append a copy of value; returns the list node on success. */
TINYCORE_API TinyNode *StringArray_Add(StringArray *array, const char *value);

TINYCORE_API uint32_t StringArray_GetSize(const StringArray *array);
TINYCORE_API const char *StringArray_GetAt(const StringArray *array, uint32_t index);

TINYCORE_END_DECLS

#endif /* TINYCORE_STRING_ARRAY_H */
