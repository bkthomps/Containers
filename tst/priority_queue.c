#include "test.h"
#include "../src/vector.h"
#include "../src/priority_queue.h"

/*
 * Include this for the stubs.
 */
struct internal_priority_queue {
    vector data;
    size_t data_size;
    int (*comparator)(const void *const one, const void *const two);
};

static void priority_queue_verify(priority_queue me)
{
    void *const vector_storage = vector_get_data(me->data);
    const int size = vector_size(me->data);
    for (int i = 0; i < size; i++) {
        const int val = *(int *) (vector_storage + i * me->data_size);
        const int left_child = 2 * i + 1;
        const int right_child = 2 * i + 2;
        if (left_child < size) {
            void *left_data = vector_storage + left_child * me->data_size;
            const int left_val = *(int *) left_data;
            assert(val >= left_val);
        }
        if (right_child < size) {
            void *right_data = vector_storage + right_child * me->data_size;
            const int right_val = *(int *) right_data;
            assert(val >= right_val);
        }
    }
}

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

int stub_priority_queue_push(priority_queue me, void *const data)
{
    const int ret = priority_queue_push(me, data);
    priority_queue_verify(me);
    return ret;
}

bool stub_priority_queue_pop(void *const data, priority_queue me)
{
    const bool ret = priority_queue_pop(data, me);
    priority_queue_verify(me);
    return ret;
}

void test_priority_queue(void)
{
    assert(!priority_queue_init(0, compare_int));
    assert(!priority_queue_init(sizeof(int), NULL));
    priority_queue me = priority_queue_init(sizeof(int), compare_int);
    assert(me);
    assert(priority_queue_size(me) == 0);
    assert(priority_queue_is_empty(me));
    int item = 5;
    stub_priority_queue_push(me, &item);
    item = 2;
    stub_priority_queue_push(me, &item);
    item = 7;
    stub_priority_queue_push(me, &item);
    item = 3;
    stub_priority_queue_push(me, &item);
    item = 4;
    stub_priority_queue_push(me, &item);
    item = 5;
    stub_priority_queue_push(me, &item);
    item = 9;
    stub_priority_queue_push(me, &item);
    item = 2;
    stub_priority_queue_push(me, &item);
    item = 3;
    stub_priority_queue_push(me, &item);
    item = -5;
    stub_priority_queue_push(me, &item);
    item = 7;
    stub_priority_queue_push(me, &item);
    item = 3;
    stub_priority_queue_push(me, &item);
    item = 4;
    stub_priority_queue_push(me, &item);
    item = 3;
    stub_priority_queue_push(me, &item);
    item = 11;
    stub_priority_queue_push(me, &item);
    item = 6;
    stub_priority_queue_push(me, &item);
    assert(priority_queue_size(me) == 16);
    assert(!priority_queue_is_empty(me));
    item = 0xdeadbeef;
    priority_queue_front(&item, me);
    assert(item == 11);
    item = 0xdeadbeef;
    stub_priority_queue_pop(&item, me);
    assert(item == 11);
    assert(priority_queue_size(me) == 15);
    item = 0xdeadbeef;
    priority_queue_front(&item, me);
    assert(item == 9);
    int latest = item;
    for (int i = 0; i < 15; i++) {
        stub_priority_queue_pop(&item, me);
        assert(item <= latest);
        latest = item;
        assert(priority_queue_size(me) == 15 - i - 1);
    }
    priority_queue_clear(me);
    assert(priority_queue_is_empty(me));
    item = 0xdeadbeef;
    assert(!priority_queue_front(&item, me));
    assert(item == 0xdeadbeef);
    assert(priority_queue_is_empty(me));
    me = priority_queue_destroy(me);
    assert(!me);
}
