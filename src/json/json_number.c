/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <tinycore/json_number.h>
#include <tinycore/tiny_alloc.h>

JsonNumber *JsonNumber_NewInteger(int32_t value)
{
    JsonNumber *number = (JsonNumber *)tiny_malloc(sizeof(JsonNumber));

    if (number == NULL)
    {
        return NULL;
    }

    number->type = JSON_NUMBER_INTEGER;
    number->value.intValue = value;
    return number;
}

JsonNumber *JsonNumber_NewFloat(double value)
{
    JsonNumber *number = (JsonNumber *)tiny_malloc(sizeof(JsonNumber));

    if (number == NULL)
    {
        return NULL;
    }

    number->type = JSON_NUMBER_FLOAT;
    number->value.floatValue = value;
    return number;
}

void JsonNumber_Delete(JsonNumber *number)
{
    tiny_free(number);
}

bool JsonNumber_Equals(const JsonNumber *number, const JsonNumber *other)
{
    double left = 0.0;
    double right = 0.0;

    if (number == other)
    {
        return true;
    }

    if (number == NULL || other == NULL)
    {
        return false;
    }

    if (number->type == JSON_NUMBER_INTEGER && other->type == JSON_NUMBER_INTEGER)
    {
        return number->value.intValue == other->value.intValue;
    }

    left = (number->type == JSON_NUMBER_INTEGER)
        ? (double)number->value.intValue
        : number->value.floatValue;
    right = (other->type == JSON_NUMBER_INTEGER)
        ? (double)other->value.intValue
        : other->value.floatValue;

    return left == right;
}

bool JsonNumber_IsInteger(const JsonNumber *number)
{
    return number != NULL && number->type == JSON_NUMBER_INTEGER;
}

int32_t JsonNumber_GetInteger(const JsonNumber *number)
{
    if (number == NULL)
    {
        return 0;
    }

    return (number->type == JSON_NUMBER_INTEGER)
        ? number->value.intValue
        : (int32_t)number->value.floatValue;
}

double JsonNumber_GetFloat(const JsonNumber *number)
{
    if (number == NULL)
    {
        return 0.0;
    }

    return (number->type == JSON_NUMBER_INTEGER)
        ? (double)number->value.intValue
        : number->value.floatValue;
}
