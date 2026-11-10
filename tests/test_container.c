/*
 * TinyCore - cross-platform core library
 *
 * Copyright (c) 2017 loachtech
 * Copyright (c) 2024-2026 openxiot contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "tiny_test.h"

#include <string.h>

#include <tinycore/tiny_list.h>
#include <tinycore/tiny_map.h>
#include <tinycore/tiny_map_item.h>
#include <tinycore/tiny_queue.h>

static int g_deleted = 0;

static void count_deleted(void *data, void *context)
{
    (void)data;
    (void)context;
    ++g_deleted;
}

static bool count_visits(void *data, void *context)
{
    int *count = (int *)context;

    (void)data;
    ++(*count);
    return true;
}

static bool stop_after_two(void *data, void *context)
{
    int *count = (int *)context;

    (void)data;
    ++(*count);
    return *count < 2;
}

static bool count_map_visits(const char *key, void *value, void *context)
{
    int *count = (int *)context;

    (void)key;
    (void)value;
    ++(*count);
    return true;
}

static bool stop_map_after_two(const char *key, void *value, void *context)
{
    int *count = (int *)context;

    (void)key;
    (void)value;
    ++(*count);
    return *count < 2;
}

static void test_list(void)
{
    TinyList list;
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int visits = 0;
    void *old = NULL;

    TinyList_Construct(&list);
    TinyList_SetDeleteListener(&list, count_deleted, NULL);

    TINY_TEST_CHECK(TinyList_IsEmpty(&list));
    TINY_TEST_CHECK(TinyList_AddTail(&list, &a) != NULL);
    TINY_TEST_CHECK(TinyList_AddTail(&list, &b) != NULL);
    TINY_TEST_CHECK(TinyList_AddHead(&list, &c) != NULL);
    TINY_TEST_CHECK(TinyList_GetSize(&list) == 3);
    TINY_TEST_CHECK(TinyList_GetHead(&list) == &c);
    TINY_TEST_CHECK(TinyList_GetTail(&list) == &b);
    TINY_TEST_CHECK(TinyList_GetAt(&list, 0) == &c);
    TINY_TEST_CHECK(TinyList_GetAt(&list, 1) == &a);
    TINY_TEST_CHECK(TinyList_GetAt(&list, 2) == &b);
    TINY_TEST_CHECK(TinyList_GetAt(&list, 3) == NULL);

    TINY_TEST_CHECK(TinyList_SetAt(&list, 1, &d, &old));
    TINY_TEST_CHECK(old == &a);
    TINY_TEST_CHECK(TinyList_GetAt(&list, 1) == &d);

    TINY_TEST_CHECK(TinyList_Remove(&list, &d) == &d);
    TINY_TEST_CHECK(TinyList_GetSize(&list) == 2);

    TINY_TEST_CHECK(TinyList_RemoveAt(&list, 0) == &c);
    TINY_TEST_CHECK(TinyList_GetSize(&list) == 1);
    TINY_TEST_CHECK(!TinyList_SetAt(&list, 5, &a, NULL));

    TINY_TEST_CHECK(TinyList_AddHead(&list, &a) != NULL);
    TINY_TEST_CHECK(TinyList_AddHead(&list, &c) != NULL);
    TINY_TEST_CHECK(TinyList_AddHead(&list, &d) != NULL);

    TinyList_Foreach(&list, count_visits, &visits);
    TINY_TEST_CHECK(visits == 4);

    visits = 0;
    TinyList_Foreach(&list, stop_after_two, &visits);
    TINY_TEST_CHECK(visits == 2);

    g_deleted = 0;
    TinyList_Dispose(&list);
    TINY_TEST_CHECK(g_deleted == 4);
    TINY_TEST_CHECK(TinyList_IsEmpty(&list));
}

static void test_map(void)
{
    TinyMap map;
    int a = 1;
    int b = 2;
    int c = 3;
    void *old = NULL;
    int visits = 0;

    TinyMap_Construct(&map);
    TinyMap_SetDeleteListener(&map, count_deleted, NULL);

    TINY_TEST_CHECK(TinyMap_Insert(&map, "a", &a) == TINY_RET_OK);
    TINY_TEST_CHECK(TinyMap_Insert(&map, "b", &b) == TINY_RET_OK);
    TINY_TEST_CHECK(TinyMap_Insert(&map, "a", &c) == TINY_RET_E_EXIST);
    TINY_TEST_CHECK(TinyMap_GetSize(&map) == 2);
    TINY_TEST_CHECK(TinyMap_GetValue(&map, "a") == &a);
    TINY_TEST_CHECK(TinyMap_GetValue(&map, "missing") == NULL);

    TINY_TEST_CHECK(TinyMap_Put(&map, "a", &c, &old) == TINY_RET_OK);
    TINY_TEST_CHECK(old == &a);
    TINY_TEST_CHECK(TinyMap_GetValue(&map, "a") == &c);

    TINY_TEST_CHECK(TinyMap_ContainsKey(&map, "b"));
    TINY_TEST_CHECK(!TinyMap_ContainsKey(&map, "c"));

    TINY_TEST_CHECK(strcmp(TinyMap_GetItemAt(&map, 0)->key, "a") == 0);
    TINY_TEST_CHECK(strcmp(TinyMap_GetItemAt(&map, 1)->key, "b") == 0);
    TINY_TEST_CHECK(TinyMap_GetItemAt(&map, 2) == NULL);

    TinyMap_Foreach(&map, count_map_visits, &visits);
    TINY_TEST_CHECK(visits == 2);

    g_deleted = 0;
    TINY_TEST_CHECK(TinyMap_Erase(&map, "a") == TINY_RET_OK);
    TINY_TEST_CHECK(g_deleted == 1);
    TINY_TEST_CHECK(TinyMap_Erase(&map, "a") == TINY_RET_E_NOT_FOUND);
    TINY_TEST_CHECK(TinyMap_GetSize(&map) == 1);

    TinyMap_Dispose(&map);
    TINY_TEST_CHECK(g_deleted == 2);
    TINY_TEST_CHECK(TinyMap_GetSize(&map) == 0);
}

static void test_queue(void)
{
    TinyQueue queue;
    int a = 1;
    int b = 2;
    int c = 3;

    TinyQueue_Construct(&queue);

    TINY_TEST_CHECK(TinyQueue_IsEmpty(&queue));
    TINY_TEST_CHECK(TinyQueue_Push(&queue, &a));
    TINY_TEST_CHECK(TinyQueue_Push(&queue, &b));
    TINY_TEST_CHECK(TinyQueue_Push(&queue, &c));
    TINY_TEST_CHECK(TinyQueue_GetSize(&queue) == 3);
    TINY_TEST_CHECK(TinyQueue_Head(&queue) == &a);
    TINY_TEST_CHECK(TinyQueue_Pop(&queue) == &a);
    TINY_TEST_CHECK(TinyQueue_Pop(&queue) == &b);
    TINY_TEST_CHECK(TinyQueue_Pop(&queue) == &c);
    TINY_TEST_CHECK(TinyQueue_Pop(&queue) == NULL);
    TINY_TEST_CHECK(TinyQueue_IsEmpty(&queue));

    TinyQueue_Dispose(&queue);
}

static void test_list_positions(void)
{
    TinyList list;
    TinyList *heap = NULL;
    TinyNode *head = NULL;
    TinyNode *tail = NULL;
    TinyNode *node_b = NULL;
    TinyNode *node_a = NULL;
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int e = 5;

    TinyList_Construct(&list);

    TINY_TEST_CHECK(TinyList_AddTail(&list, &a) != NULL);
    node_b = TinyList_AddTail(&list, &b);
    TINY_TEST_CHECK(node_b != NULL);
    TINY_TEST_CHECK(TinyList_AddTail(&list, &c) != NULL);

    /* Insert after the middle node: a b d c */
    TINY_TEST_CHECK(TinyList_InsertAfter(&list, node_b, &d) != NULL);
    TINY_TEST_CHECK(TinyList_GetAt(&list, 2) == &d);
    TINY_TEST_CHECK(TinyList_GetSize(&list) == 4);

    /* Insert before the head: e a b d c */
    node_a = TinyList_GetPositionAt(&list, 0);
    TINY_TEST_CHECK(node_a != NULL && node_a->data == &a);
    TINY_TEST_CHECK(TinyList_InsertBefore(&list, node_a, &e) != NULL);
    TINY_TEST_CHECK(TinyList_GetSize(&list) == 5);
    TINY_TEST_CHECK(TinyList_GetAt(&list, 0) == &e);
    TINY_TEST_CHECK(TinyList_GetAt(&list, 1) == &a);

    head = TinyList_GetHeadPosition(&list);
    tail = TinyList_GetTailPosition(&list);
    TINY_TEST_CHECK(head != NULL && head->data == &e);
    TINY_TEST_CHECK(tail != NULL && tail->data == &c);
    TINY_TEST_CHECK(head != NULL && TinyList_GetNext(head) != NULL);
    TINY_TEST_CHECK(head != NULL && TinyList_GetNext(head)->data == &a);
    TINY_TEST_CHECK(head != NULL && TinyList_GetPrev(head) == NULL);
    TINY_TEST_CHECK(tail != NULL && TinyList_GetPrev(tail) != NULL);
    TINY_TEST_CHECK(tail != NULL && TinyList_GetPrev(tail)->data == &d);
    TINY_TEST_CHECK(TinyList_GetNext(NULL) == NULL);
    TINY_TEST_CHECK(TinyList_GetPrev(NULL) == NULL);

    TINY_TEST_CHECK(TinyList_InsertAfter(&list, NULL, &a) == NULL);
    TINY_TEST_CHECK(TinyList_InsertBefore(&list, NULL, &a) == NULL);
    TINY_TEST_CHECK(TinyList_AddTail(NULL, &a) == NULL);
    TINY_TEST_CHECK(TinyList_AddHead(NULL, &a) == NULL);

    TINY_TEST_CHECK(TinyList_RemoveHead(&list) == &e);
    TINY_TEST_CHECK(TinyList_RemoveTail(&list) == &c);
    TINY_TEST_CHECK(TinyList_GetSize(&list) == 3);
    TINY_TEST_CHECK(TinyList_Remove(&list, &a) == &a);
    TINY_TEST_CHECK(TinyList_Remove(&list, &a) == NULL);
    TINY_TEST_CHECK(TinyList_GetSize(&list) == 2);

    TinyList_RemoveAll(&list);
    TINY_TEST_CHECK(TinyList_IsEmpty(&list));
    TINY_TEST_CHECK(TinyList_RemoveHead(&list) == NULL);
    TINY_TEST_CHECK(TinyList_RemoveTail(&list) == NULL);
    TINY_TEST_CHECK(TinyList_Remove(NULL, &a) == NULL);
    TINY_TEST_CHECK(TinyList_RemoveHead(NULL) == NULL);
    TINY_TEST_CHECK(TinyList_RemoveTail(NULL) == NULL);
    TINY_TEST_CHECK(TinyList_GetHead(NULL) == NULL);
    TINY_TEST_CHECK(TinyList_GetTail(NULL) == NULL);
    TINY_TEST_CHECK(TinyList_GetSize(NULL) == 0);
    TINY_TEST_CHECK(TinyList_IsEmpty(NULL));
    TinyList_RemoveAll(NULL);
    TinyList_SetDeleteListener(NULL, count_deleted, NULL);
    TinyList_Foreach(NULL, count_visits, NULL);

    TinyList_Dispose(&list);

    heap = TinyList_New();
    TINY_TEST_CHECK(heap != NULL);
    if (heap != NULL)
    {
        TINY_TEST_CHECK(TinyList_AddTail(heap, &a) != NULL);
        TINY_TEST_CHECK(TinyList_GetSize(heap) == 1);
        TinyList_Delete(heap);
    }

    TinyList_Delete(NULL);
}

static void test_map_extra(void)
{
    TinyMap map;
    TinyMap *heap = NULL;
    TinyMapItem *item = NULL;
    int a = 1;
    int b = 2;
    int c = 3;
    int visits = 0;
    void *old = (void *)1;

    TinyMap_Construct(&map);
    TinyMap_SetDeleteListener(&map, count_deleted, NULL);

    TINY_TEST_CHECK(TinyMap_Put(NULL, "a", &a, NULL) == TINY_RET_E_NULL_PTR);
    TINY_TEST_CHECK(TinyMap_Put(&map, NULL, &a, NULL) == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(TinyMap_Insert(NULL, "a", &a) == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(TinyMap_Insert(&map, NULL, &a) == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(TinyMap_Erase(NULL, "a") == TINY_RET_E_PARAM_INVALID);
    TINY_TEST_CHECK(TinyMap_Erase(&map, NULL) == TINY_RET_E_PARAM_INVALID);

    TINY_TEST_CHECK(TinyMap_Insert(&map, "a", &a) == TINY_RET_OK);
    TINY_TEST_CHECK(TinyMap_Put(&map, "a", &b, &old) == TINY_RET_OK);
    TINY_TEST_CHECK(old == &a);
    TINY_TEST_CHECK(TinyMap_Put(&map, "b", &c, NULL) == TINY_RET_OK);

    /* NULL is a legal value; the key still exists. */
    old = (void *)1;
    TINY_TEST_CHECK(TinyMap_Put(&map, "nil", NULL, &old) == TINY_RET_OK);
    TINY_TEST_CHECK(old == NULL);
    TINY_TEST_CHECK(TinyMap_ContainsKey(&map, "nil"));
    TINY_TEST_CHECK(TinyMap_GetValue(&map, "nil") == NULL);

    item = TinyMap_GetItem(&map, "a");
    TINY_TEST_CHECK(item != NULL);
    TINY_TEST_CHECK(item != NULL && strcmp(TinyMapItem_GetKey(item), "a") == 0);
    TINY_TEST_CHECK(item != NULL && TinyMapItem_GetValue(item) == &b);
    TINY_TEST_CHECK(TinyMap_GetItem(&map, "missing") == NULL);
    TINY_TEST_CHECK(TinyMap_GetValue(NULL, "a") == NULL);
    TINY_TEST_CHECK(!TinyMap_ContainsKey(NULL, "a"));
    TINY_TEST_CHECK(TinyMap_GetSize(NULL) == 0);
    TINY_TEST_CHECK(TinyMap_IsEmpty(NULL));

    TinyMap_Foreach(&map, stop_map_after_two, &visits);
    TINY_TEST_CHECK(visits == 2);
    TinyMap_Foreach(&map, NULL, &visits);
    TinyMap_Foreach(NULL, count_map_visits, &visits);

    g_deleted = 0;
    TinyMap_Clear(&map);
    TINY_TEST_CHECK(TinyMap_GetSize(&map) == 0);
    TINY_TEST_CHECK(g_deleted == 3);
    TINY_TEST_CHECK(TinyMap_IsEmpty(&map));

    TinyMap_Clear(NULL);
    TinyMap_SetDeleteListener(NULL, count_deleted, NULL);
    TinyMap_Dispose(NULL);
    TinyMap_Delete(NULL);

    heap = TinyMap_New();
    TINY_TEST_CHECK(heap != NULL);
    if (heap != NULL)
    {
        TINY_TEST_CHECK(TinyMap_Insert(heap, "k", &a) == TINY_RET_OK);
        TinyMap_Delete(heap);
    }
}

static void test_map_item(void)
{
    TinyMapItem *item = TinyMapItem_New("key", NULL);
    int value = 7;

    TINY_TEST_CHECK(TinyMapItem_New(NULL, NULL) == NULL);
    TINY_TEST_CHECK(TinyMapItem_GetKey(NULL) == NULL);
    TINY_TEST_CHECK(TinyMapItem_GetValue(NULL) == NULL);
    TINY_TEST_CHECK(!TinyMapItem_KeyEquals(NULL, "key"));
    TinyMapItem_SetValue(NULL, &value);
    TinyMapItem_Delete(NULL);

    TINY_TEST_CHECK(item != NULL);
    if (item == NULL)
    {
        return;
    }

    TINY_TEST_CHECK(strcmp(TinyMapItem_GetKey(item), "key") == 0);
    TINY_TEST_CHECK(TinyMapItem_GetValue(item) == NULL);
    TINY_TEST_CHECK(TinyMapItem_KeyEquals(item, "key"));
    TINY_TEST_CHECK(!TinyMapItem_KeyEquals(item, "Key"));
    TINY_TEST_CHECK(!TinyMapItem_KeyEquals(item, NULL));

    TinyMapItem_SetValue(item, &value);
    TINY_TEST_CHECK(TinyMapItem_GetValue(item) == &value);

    TinyMapItem_Delete(item);
}

static void test_queue_extra(void)
{
    TinyQueue queue;
    TinyQueue *heap = NULL;
    int a = 1;
    int b = 2;

    heap = TinyQueue_New();
    TINY_TEST_CHECK(heap != NULL);
    if (heap != NULL)
    {
        TINY_TEST_CHECK(TinyQueue_Push(heap, &a));
        TINY_TEST_CHECK(TinyQueue_GetSize(heap) == 1);
        TinyQueue_Delete(heap);
    }

    TinyQueue_Construct(&queue);
    TINY_TEST_CHECK(TinyQueue_IsEmpty(&queue));
    TINY_TEST_CHECK(TinyQueue_Push(&queue, &a));
    TINY_TEST_CHECK(TinyQueue_Push(&queue, &b));
    TINY_TEST_CHECK(TinyQueue_GetSize(&queue) == 2);
    g_deleted = 0;
    TinyList_SetDeleteListener(&queue.list, count_deleted, NULL);
    TinyQueue_Clear(&queue);
    TINY_TEST_CHECK(g_deleted == 2);
    TINY_TEST_CHECK(TinyQueue_IsEmpty(&queue));

    TINY_TEST_CHECK(!TinyQueue_Push(NULL, &a));
    TINY_TEST_CHECK(TinyQueue_Pop(NULL) == NULL);
    TINY_TEST_CHECK(TinyQueue_Head(NULL) == NULL);
    TINY_TEST_CHECK(TinyQueue_GetSize(NULL) == 0);
    TINY_TEST_CHECK(TinyQueue_IsEmpty(NULL));
    TinyQueue_Clear(NULL);
    TinyQueue_Dispose(NULL);
    TinyQueue_Delete(NULL);

    TinyQueue_Dispose(&queue);
}

int main(void)
{
    TINY_TEST_BEGIN("container");
    test_list();
    test_list_positions();
    test_map();
    test_map_extra();
    test_map_item();
    test_queue();
    test_queue_extra();
    return TINY_TEST_FINISH();
}
