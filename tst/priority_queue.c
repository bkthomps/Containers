#include "test.h"
#include "../src/vector.h"
#include "../src/priority_queue.h"

/*
 * Include this for the stubs.
 */
struct _priority_queue {
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
    priority_queue a = priority_queue_init(sizeof(int), compare_int);
    assert(a != NULL);
    assert(priority_queue_size(a) == 0);
    assert(priority_queue_is_empty(a));
    int b = 5;
    stub_priority_queue_push(a, &b);
    b = 2;
    stub_priority_queue_push(a, &b);
    b = 7;
    stub_priority_queue_push(a, &b);
    b = 3;
    stub_priority_queue_push(a, &b);
    b = 4;
    stub_priority_queue_push(a, &b);
    b = 5;
    stub_priority_queue_push(a, &b);
    b = 9;
    stub_priority_queue_push(a, &b);
    b = 2;
    stub_priority_queue_push(a, &b);
    b = 3;
    stub_priority_queue_push(a, &b);
    b = -5;
    stub_priority_queue_push(a, &b);
    b = 7;
    stub_priority_queue_push(a, &b);
    b = 3;
    stub_priority_queue_push(a, &b);
    b = 4;
    stub_priority_queue_push(a, &b);
    b = 3;
    stub_priority_queue_push(a, &b);
    b = 11;
    stub_priority_queue_push(a, &b);
    b = 6;
    stub_priority_queue_push(a, &b);
    assert(priority_queue_size(a) == 16);
    assert(!priority_queue_is_empty(a));
    b = 0xdeadbeef;
    priority_queue_front(&b, a);
    assert(b == 11);
    b = 0xdeadbeef;
    stub_priority_queue_pop(&b, a);
    assert(b == 11);
    assert(priority_queue_size(a) == 15);
    b = 0xdeadbeef;
    priority_queue_front(&b, a);
    assert(b == 9);
    int c = b;
    for (int i = 0; i < 15; i++) {
        stub_priority_queue_pop(&b, a);
        assert(b <= c);
        c = b;
        assert(priority_queue_size(a) == 15 - i - 1);
    }
    priority_queue_clear(a);
    assert(priority_queue_is_empty(a));
    b = 0xdeadbeef;
    assert(!priority_queue_front(&b, a));
    assert(b == 0xdeadbeef);
    assert(priority_queue_is_empty(a));
    a = priority_queue_destroy(a);
    assert(a == NULL);
}
