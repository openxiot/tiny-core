/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_DECODER_H
#define TINYCORE_JSON_DECODER_H

#include <tinycore/buffer.h>
#include <tinycore/cycle_buffer.h>
#include <tinycore/json_object.h>

TINYCORE_BEGIN_DECLS

typedef enum JsonDecoderStatus
{
    JSON_DECODER_STATUS_OK         = 0,
    JSON_DECODER_STATUS_INCOMPLETE = 1,
    JSON_DECODER_STATUS_INVALID    = 2,
    JSON_DECODER_STATUS_NO_MEMORY  = 3
} JsonDecoderStatus;

/*
 * Incremental JSON object decoder.  Bytes fed through JsonDecoder_Feed()
 * (or read from a CycleBuffer) are accumulated until a complete top level
 * object is available; the object is then decoded and its bytes are
 * consumed, leaving any following document in the buffer.
 */
typedef struct JsonDecoder
{
    Buffer input;
    bool constructed;
} JsonDecoder;

TINYCORE_API TINYCORE_RET JsonDecoder_Construct(JsonDecoder *decoder);
TINYCORE_API void JsonDecoder_Dispose(JsonDecoder *decoder);
TINYCORE_API JsonDecoder *JsonDecoder_New(void);
TINYCORE_API void JsonDecoder_Delete(JsonDecoder *decoder);

/* Decode one complete document.  The decoder starts from a clean state. */
TINYCORE_API JsonObject *JsonDecoder_Parse(JsonDecoder *decoder,
                                           const char *string,
                                           JsonDecoderStatus *status);

TINYCORE_API TINYCORE_RET JsonDecoder_Feed(JsonDecoder *decoder,
                                           const void *data,
                                           size_t length);

/* Decode the next complete document from the bytes fed so far.  On success
 * the consumed bytes are removed from the internal buffer.  Returns NULL
 * with status INCOMPLETE when more input is needed. */
TINYCORE_API JsonObject *JsonDecoder_ParseBuffered(JsonDecoder *decoder,
                                                   JsonDecoderStatus *status);

/* Consume every readable byte of buffer into the decoder, then decode the
 * next complete document. */
TINYCORE_API JsonObject *JsonDecoder_ParseBuffer(JsonDecoder *decoder,
                                                 CycleBuffer *buffer,
                                                 JsonDecoderStatus *status);

TINYCORE_END_DECLS

#endif /* TINYCORE_JSON_DECODER_H */
