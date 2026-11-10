# TinyCore

TinyCore is a small, portable C99 core library for embedded and desktop
projects. It provides the building blocks that keep reappearing in firmware
and gateway code: containers, byte buffers, string helpers, TLV8 encoding,
JSON, a thread wrapper, IEEE 754 helpers, random numbers and logging.

The library has no mandatory dependencies beyond a C99 standard library.
POSIX threads and Win32 threads are used only by the optional thread module.

## Layout

```
include/tinycore/   public headers, one per module (plus umbrella headers)
src/                implementation, grouped by module
tests/              self contained test suite (CTest)
examples/           small runnable examples
cmake/              package config templates
```

Every header is installed under `tinycore/`, so users include either one
module, e.g. `#include <tinycore/json.h>`, or everything at once:

```c
#include <tinycore/tinycore.h>
```

## Building

Requirements: CMake 3.16 or newer and a C99 compiler.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### CLion

Open the repository root in CLion. CLion detects `CMakeLists.txt`
automatically; select the `Debug` (or `Release`) CMake profile and reload.
Tests are registered with CTest, so they appear in the run configuration
list and can be executed one by one or through `ctest`.

### CMake options

| Option | Default | Meaning |
| --- | --- | --- |
| `BUILD_SHARED_LIBS` | `OFF` | Build a shared library instead of a static one. |
| `TINYCORE_BUILD_TESTS` | top level only | Build the test suite. |
| `TINYCORE_BUILD_EXAMPLES` | `OFF` | Build the examples. |
| `TINYCORE_ENABLE_DEBUG_LOG` | `OFF` | Define `TINY_DEBUG=1` (asserts and log output). |
| `TINYCORE_ENABLE_SANITIZERS` | `OFF` | Build with AddressSanitizer and UBSan. |
| `TINYCORE_INSTALL` | top level only | Generate install and package config rules. |
| `TINYCORE_WARNINGS_AS_ERRORS` | `OFF` | Promote compiler warnings to errors. |

Install and consume from another CMake project:

```sh
cmake --install build --prefix /some/prefix
```

```cmake
find_package(tinycore CONFIG REQUIRED)
target_link_libraries(app PRIVATE tinycore::tinycore)
```

A pkg-config file (`tinycore.pc`) is installed as well.

## Modules

| Module | Headers | Contents |
| --- | --- | --- |
| Base | `tiny_base.h`, `tiny_ret.h`, `tiny_platform.h`, `tiny_typedef.h` | Platform/compiler detection, result type, common macros. |
| Portable | `tiny_alloc.h`, `tiny_strdup.h`, `tiny_time.h` | Allocation wrappers, string duplication, monotonic time. |
| Containers | `tiny_list.h`, `tiny_map.h`, `tiny_queue.h` | Doubly linked list, insertion ordered string map, queue. |
| Buffers | `buffer.h`, `byte_buffer.h`, `cycle_buffer.h` | Growable buffer, linear byte buffer, ring buffer. |
| Strings | `tiny_str_equal.h`, `tiny_str_split.h`, `tiny_strnlen.h`, `tiny_url_split.h`, `tiny_buffer_append.h`, `string_array.h`, `tiny_char_util.h` | Locale independent helpers and a string array. |
| JSON | `json.h` | Strict RFC 8259 object model, tokenizer, decoder (including incremental streaming) and encoder. |
| TLV8 | `tlv8.h` | TLV8 items and lists with 255 byte fragmentation, plus JSON value conversion. |
| Miscellaneous | `ieee754.h`, `tiny_random.h`, `tiny_thread.h`, `tiny_log.h` | IEEE 754 wire encoding, PCG32 random numbers, a minimal thread wrapper, logging. |

The BSON codec of the original project is intentionally not part of this
port; the public API is JSON only. BSON can be added later as another
`src/bson/` module without changing the existing headers.

## API conventions

* Types and functions are prefixed with the module name, e.g.
  `JsonObject_PutValue`, `TinyList_AddTail`, `tiny_str_equal`.
* Every collection supports the same lifecycle:
  `X_Construct()` / `X_Dispose()` for stack instances and
  `X_New()` / `X_Delete()` for heap instances.
* `Put*`, `Add*` and `Set*` take ownership of the values they are given;
  `Get*` returns borrowed pointers. A replace drops the previous value
  through the map/list delete listener.
* Errors are reported with `TINYCORE_RET` (an `int32_t`). Test for success
  with `TINYCORE_RET_SUCCEEDED(ret)`; `TINYCORE_RET_FAILED(ret)` is the
  inverse. Legacy `RET_SUCCEEDED` / `RET_FAILED` aliases exist for callers
  coming from the older code base.
* Functions taking an output buffer always return the number of bytes
  written and never write past the given capacity.

### Custom allocators

All allocations are routed through `tiny_malloc()`, `tiny_calloc()`,
`tiny_realloc()` and `tiny_free()`. A host project can provide its own
definitions of these four functions to use a RTOS heap or a pool allocator;
TinyCore itself always pairs its allocations with `tiny_free()`.

### Portability notes

* C99 is required; the code builds cleanly with `-Wall -Wextra -Wpedantic
  -Wconversion -Wsign-conversion -Wshadow -Wcast-qual`.
* `tiny_char_util.h` is locale independent: it never calls `<ctype.h>`, so
  behavior is identical in hosted and freestanding environments.
* `tiny_thread.h` uses Win32 threads on Windows, pthreads where available
  (`TINYCORE_PLATFORM_POSIX`), and reports `TINY_RET_E_NOT_SUPPORT`
  elsewhere. Define `TINYCORE_NO_PTHREAD` to disable pthread detection.
* `ieee754.h` verifies at compile time that `float` is 32-bit and `double`
  is 64-bit; `encode_f32` / `decode_f32` use big-endian wire order.
* The tests avoid directory iteration and hard-code their resource file
  names, so they run on platforms without `<dirent.h>`.

## Tests

```sh
ctest --test-dir build --output-on-failure
```

For a sanitizer build:

```sh
cmake -S . -B build-asan -DTINYCORE_ENABLE_SANITIZERS=ON
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
```

The JSON suite in `tests/test_json_resources.c` uses the fixtures under
`tests/resources/json/` (originally from the loachtech TinyCore tests).

## License

MIT. See [LICENSE](LICENSE).
