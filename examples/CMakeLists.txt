# ---------------------------------------------------------------------------
# TinyCore examples
# ---------------------------------------------------------------------------

add_executable(tinycore_json_demo json_demo.c)
target_link_libraries(tinycore_json_demo PRIVATE tinycore)

if(NOT MSVC)
    target_compile_options(tinycore_json_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()
