cmake_minimum_required(VERSION 3.16)

project(tinycore
        VERSION 1.0.0
        DESCRIPTION "TinyCore - a small, portable C core library for embedded systems"
        HOMEPAGE_URL "https://github.com/openxiot/tiny-core"
        LANGUAGES C)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build tinycore as a shared library" OFF)
option(TINYCORE_BUILD_TESTS "Build the TinyCore test suite" ${PROJECT_IS_TOP_LEVEL})
option(TINYCORE_BUILD_EXAMPLES "Build TinyCore examples" OFF)
option(TINYCORE_ENABLE_DEBUG_LOG "Enable TINY_DEBUG logging and assertions" OFF)
option(TINYCORE_ENABLE_SANITIZERS "Build with Address/UB sanitizers (debug builds)" OFF)
option(TINYCORE_INSTALL "Generate install rules" ${PROJECT_IS_TOP_LEVEL})
option(TINYCORE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS ON)
endif()

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------
set(TINYCORE_SOURCES
        src/base/tiny_ret.c

        src/portable/tiny_alloc.c
        src/portable/tiny_strdup.c
        src/portable/tiny_time.c

        src/log/tiny_log.c
        src/log/tiny_print_binary.c

        src/container/tiny_list.c
        src/container/tiny_map.c
        src/container/tiny_map_item.c
        src/container/tiny_queue.c

        src/buffer/buffer.c
        src/buffer/byte_buffer.c
        src/buffer/cycle_buffer.c

        src/string/tiny_char_util.c
        src/string/tiny_str_equal.c
        src/string/tiny_str_split.c
        src/string/tiny_strnlen.c
        src/string/tiny_url_split.c
        src/string/tiny_buffer_append.c
        src/string/string_array.c

        src/ieee754/ieee754.c
        src/random/tiny_random.c
        src/thread/tiny_thread.c

        src/tlv8/tlv8_item.c
        src/tlv8/tlv8_list.c
        src/tlv8/tlv8_item_factory.c

        src/json/json_definitions.c
        src/json/json_value.c
        src/json/json_boolean.c
        src/json/json_string.c
        src/json/json_number.c
        src/json/json_array.c
        src/json/json_object.c
        src/json/json_token.c
        src/json/json_tokenizer.c
        src/json/json_decoder.c
        src/json/json_encoder.c)

add_library(tinycore ${TINYCORE_SOURCES})
add_library(tinycore::tinycore ALIAS tinycore)

set_target_properties(tinycore PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS ON
        POSITION_INDEPENDENT_CODE ON
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR})

target_include_directories(tinycore
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

# The export macro needs to know whether the library is built as a DLL.
if(BUILD_SHARED_LIBS)
    target_compile_definitions(tinycore
            PRIVATE TINYCORE_EXPORTS
            PUBLIC TINYCORE_SHARED)
    set_target_properties(tinycore PROPERTIES
            C_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN YES)
endif()

if(TINYCORE_ENABLE_DEBUG_LOG)
    target_compile_definitions(tinycore PUBLIC TINY_DEBUG=1)
endif()

if(TINYCORE_WARNINGS_AS_ERRORS)
    target_compile_definitions(tinycore PRIVATE TINYCORE_WARNINGS_AS_ERRORS=1)
endif()

if(NOT WIN32)
    find_package(Threads REQUIRED)
    target_link_libraries(tinycore PUBLIC Threads::Threads)
endif()

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(tinycore PRIVATE /W4)
    if(TINYCORE_WARNINGS_AS_ERRORS)
        target_compile_options(tinycore PRIVATE /WX)
    endif()
else()
    target_compile_options(tinycore PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wcast-qual
            -Wformat=2
            -Wundef
            -Wpointer-arith
            -Wstrict-prototypes
            -Wmissing-prototypes)
    if(TINYCORE_WARNINGS_AS_ERRORS)
        target_compile_options(tinycore PRIVATE -Werror)
    endif()
endif()

if(TINYCORE_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(tinycore PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(tinycore PRIVATE -fsanitize=address,undefined)
endif()

# ---------------------------------------------------------------------------
# Tests / examples
# ---------------------------------------------------------------------------
if(TINYCORE_BUILD_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

if(TINYCORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ---------------------------------------------------------------------------
# Install / package config
# ---------------------------------------------------------------------------
if(TINYCORE_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS tinycore
            EXPORT tinycoreTargets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(DIRECTORY include/tinycore
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.h")

    install(EXPORT tinycoreTargets
            FILE tinycoreTargets.cmake
            NAMESPACE tinycore::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinycore)

    configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tinycoreConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/tinycoreConfig.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinycore)

    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/tinycoreConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/tinycoreConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/tinycoreConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tinycore)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/tinycore.pc.in
            ${CMAKE_CURRENT_BINARY_DIR}/tinycore.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tinycore.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
