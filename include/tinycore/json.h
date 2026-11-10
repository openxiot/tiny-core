/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TINYCORE_JSON_H
#define TINYCORE_JSON_H

/*
 * Umbrella header for the JSON module.
 *
 *   JsonValue / JsonString / JsonNumber / JsonBoolean / JsonArray / JsonObject
 *       Object model with owned, reference-free payloads.
 *
 *   JsonTokenizer / JsonDecoder
 *       Strict RFC 8259 tokenizer and a decoder that can also consume
 *       documents incrementally from a stream.
 *
 *   JsonEncoder
 *       Compact or pretty printer.
 */

#include <tinycore/json_definitions.h>
#include <tinycore/json_value.h>
#include <tinycore/json_string.h>
#include <tinycore/json_number.h>
#include <tinycore/json_boolean.h>
#include <tinycore/json_array.h>
#include <tinycore/json_object.h>
#include <tinycore/json_token.h>
#include <tinycore/json_tokenizer.h>
#include <tinycore/json_decoder.h>
#include <tinycore/json_encoder.h>

#endif /* TINYCORE_JSON_H */
