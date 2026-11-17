/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_TOKENIZER_H
#define TINYCORE_JSON_TOKENIZER_H

#include <tinycore/json_token.h>
#include <tinycore/tiny_queue.h>

TINYCORE_BEGIN_DECLS

typedef struct JsonTokenizer
{
    TinyQueue queue; /* JsonToken* */
    const char *string;
    size_t length;
} JsonTokenizer;

TINYCORE_API TinyRet JsonTokenizer_Construct(JsonTokenizer *tokenizer);
TINYCORE_API void JsonTokenizer_Dispose(JsonTokenizer *tokenizer);

/* Tokenize the complete document held in string/length.  The tokenizer
 * validates token level syntax, not the object/array structure. */
TINYCORE_API TinyRet JsonTokenizer_Parse(JsonTokenizer *tokenizer,
                                              const char *string,
                                              size_t length);

TINYCORE_API JsonToken *JsonTokenizer_Head(const JsonTokenizer *tokenizer);
TINYCORE_API void JsonTokenizer_Pop(JsonTokenizer *tokenizer);
TINYCORE_API uint32_t JsonTokenizer_GetSize(const JsonTokenizer *tokenizer);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_TOKENIZER_H */
