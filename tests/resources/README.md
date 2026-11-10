# Test resources

The JSON fixtures under `json/` come from the loachtech TinyCore project
(`test/Json/resources/`) and are used here to verify that the ported decoder
and encoder keep byte-for-byte compatible output.

* `buffer/` — compact documents; the encoder must reproduce each file exactly.
* `string/` — pretty printed documents, including a full HomeKit accessory
  description and an event message.
* `float/` — floating point values that must survive decoding.
* `part/` — streams containing several documents, optionally ending with an
  incomplete one, used by the incremental decoder tests.
* `equals/` — pairs of objects that must compare equal regardless of key
  order.
* `invalid/` — documents that must be rejected.
* `utf8/` — non-ASCII keys and values.

The fixtures retain their original MIT license terms.
