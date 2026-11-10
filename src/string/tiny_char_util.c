/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/tiny_char_util.h>

bool tiny_is_ascii(int c)
{
    return c >= 0 && c <= 0x7F;
}

bool tiny_is_control(int c)
{
    return (c >= 0 && c <= 0x1F) || c == 0x7F;
}

bool tiny_is_tspecial(int c)
{
    switch (c)
    {
        case '(':
        case ')':
        case '<':
        case '>':
        case '@':
        case ',':
        case ';':
        case ':':
        case '\\':
        case '"':
        case '/':
        case '[':
        case ']':
        case '?':
        case '=':
        case '{':
        case '}':
        case ' ':
        case '\t':
            return true;

        default:
            return false;
    }
}

bool tiny_is_digit(int c)
{
    return c >= '0' && c <= '9';
}

bool tiny_is_hex(char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}
