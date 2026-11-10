# ---------------------------------------------------------------------------
# TinyCore tests
# ---------------------------------------------------------------------------

set(TINYCORE_TEST_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources/json")

function(tinycore_add_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE tinycore)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(${name} PRIVATE
            TINYCORE_TEST_RESOURCE_DIR="${TINYCORE_TEST_RESOURCE_DIR}")

    if(MSVC)
        target_compile_options(${name} PRIVATE /W4)
    else()
        target_compile_options(${name} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wshadow
                -Wstrict-prototypes
                -Wmissing-prototypes)

        if(TINYCORE_ENABLE_SANITIZERS)
            target_compile_options(${name} PRIVATE
                    -fsanitize=address,undefined
                    -fno-omit-frame-pointer)
            target_link_options(${name} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()

    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES TIMEOUT 120)
endfunction()

tinycore_add_test(test_base test_base.c)
tinycore_add_test(test_container test_container.c)
tinycore_add_test(test_buffer test_buffer.c)
tinycore_add_test(test_string test_string.c)
tinycore_add_test(test_ieee754 test_ieee754.c)
tinycore_add_test(test_random test_random.c)
tinycore_add_test(test_thread test_thread.c)
tinycore_add_test(test_tlv8 test_tlv8.c)
tinycore_add_test(test_json test_json.c)
tinycore_add_test(test_json_model test_json_model.c)
tinycore_add_test(test_json_resources test_json_resources.c)
