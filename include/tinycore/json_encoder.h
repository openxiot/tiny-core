/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_ENCODER_H
#define TINYCORE_JSON_ENCODER_H

#include <tinycore/buffer.h>
#include <tinycore/json_definitions.h>

TINYCORE_BEGIN_DECLS

/* Receives the encoded text in order.  Return a failed TinyRet to
 * abort encoding. */
typedef TinyRet (*JsonOutput)(const char *data, size_t length, void *context);

TINYCORE_API TinyRet JsonEncoder_Encode(const JsonObject *object,
                                             bool pretty,
                                             JsonOutput output,
                                             void *context);

TINYCORE_API TinyRet JsonEncoder_EncodeToBuffer(const JsonObject *object,
                                                     bool pretty,
                                                     Buffer *buffer);

/* Encode into a newly allocated NUL terminated string; release it with
 * tiny_free().  Returns NULL on failure. */
TINYCORE_API char *JsonEncoder_ToString(const JsonObject *object, bool pretty);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_ENCODER_H */
