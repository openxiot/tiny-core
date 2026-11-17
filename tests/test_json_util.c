/* test: JsonArray_CopyInto — in-place deep copy of an array */

#include "tiny_test.h"

#include <string.h>

#include <tinycore/json_array.h>
#include <tinycore/json_boolean.h>
#include <tinycore/json_number.h>
#include <tinycore/json_string.h>
#include <tinycore/json_value.h>

static void test_null_arguments(void)
{
    JsonArray array;

    TINY_TEST_CHECK(JsonArray_Construct(&array) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonArray_CopyInto(NULL, &array) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(JsonArray_CopyInto(&array, NULL) == TINY_RET_E_NULL_PTR);

    JsonArray_Dispose(&array);
}

static void test_copy_empty(void)
{
    JsonArray dst;
    JsonArray src;

    TINY_TEST_CHECK(JsonArray_Construct(&dst) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_Construct(&src) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonArray_CopyInto(&dst, &src) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_GetSize(&dst) == 0);
    TINY_TEST_CHECK(JsonArray_IsEmpty(&dst));

    JsonArray_Dispose(&dst);
    JsonArray_Dispose(&src);
}

static void test_copy_values(void)
{
    JsonArray dst;
    JsonArray src;

    TINY_TEST_CHECK(JsonArray_Construct(&dst) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_Construct(&src) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonArray_AddString(&src, "hello") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddInteger(&src, 42) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddBoolean(&src, true) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonArray_CopyInto(&dst, &src) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_GetSize(&dst) == 3);

    JsonString *s = JsonArray_GetString(&dst, 0);
    TINY_TEST_CHECK(s != NULL && strcmp(JsonString_GetValue(s), "hello") == 0);
    JsonNumber *n = JsonArray_GetNumber(&dst, 1);
    TINY_TEST_CHECK(n != NULL && JsonNumber_GetInteger(n) == 42);
    JsonBoolean *b = JsonArray_GetBoolean(&dst, 2);
    TINY_TEST_CHECK(b != NULL && b->value == true);

    JsonArray_Dispose(&dst);
    JsonArray_Dispose(&src);
}

static void test_deep_copy_independent(void)
{
    JsonArray dst;
    JsonArray src;

    TINY_TEST_CHECK(JsonArray_Construct(&dst) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_Construct(&src) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonArray_AddString(&src, "a") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_CopyInto(&dst, &src) == TINY_RET_OK);

    /* Mutating the source later must not affect the copy. */
    TINY_TEST_CHECK(JsonArray_AddInteger(&dst, 7) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_GetSize(&dst) == 2);
    TINY_TEST_CHECK(JsonArray_GetSize(&src) == 1);

    JsonArray_Dispose(&dst);
    JsonArray_Dispose(&src);
}

static void test_overwrite_existing_content(void)
{
    JsonArray dst;
    JsonArray src;

    TINY_TEST_CHECK(JsonArray_Construct(&dst) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_Construct(&src) == TINY_RET_OK);

    TINY_TEST_CHECK(JsonArray_AddString(&dst, "old") == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_AddString(&src, "new") == TINY_RET_OK);

    TINY_TEST_CHECK(JsonArray_CopyInto(&dst, &src) == TINY_RET_OK);
    TINY_TEST_CHECK(JsonArray_GetSize(&dst) == 1);

    JsonString *s = JsonArray_GetString(&dst, 0);
    TINY_TEST_CHECK(s != NULL && strcmp(JsonString_GetValue(s), "new") == 0);

    JsonArray_Dispose(&dst);
    JsonArray_Dispose(&src);
}

int main(void)
{
    TINY_TEST_BEGIN("json_util");
    test_null_arguments();
    test_copy_empty();
    test_copy_values();
    test_deep_copy_independent();
    test_overwrite_existing_content();
    return TINY_TEST_FINISH();
}