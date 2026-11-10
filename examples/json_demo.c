/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

#include <tinycore/json.h>
#include <tinycore/tiny_alloc.h>

int main(void)
{
    static const char document[] =
        "{\"device\":\"sensor-1\",\"online\":true,\"values\":[21.5,22.0,21.8]}";
    JsonObject *object = JsonObject_NewString(document);
    char *pretty = NULL;

    if (object == NULL)
    {
        fprintf(stderr, "failed to parse document\n");
        return 1;
    }

    printf("device: %s\n", JsonObject_GetStringValue(object, "device"));
    printf("online: %s\n", JsonBoolean_GetValue(JsonObject_GetBoolean(object, "online")) ? "yes" : "no");

    pretty = JsonObject_ToString(object, true);
    if (pretty != NULL)
    {
        printf("%s\n", pretty);
        tiny_free(pretty);
    }

    JsonObject_Delete(object);
    return 0;
}
