#include "test.h"
#include "../src/priority_queue.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void test_priority_queue(void)
{
    priority_queue a = priority_queue_init(sizeof(int), compare_int);
    assert(a != NULL);
    assert(priority_queue_size(a) == 0);
    assert(priority_queue_is_empty(a));
    int b = 5;
    priority_queue_push(a, &b);
    b = 2;
    priority_queue_push(a, &b);
    b = 7;
    priority_queue_push(a, &b);
    b = 3;
    priority_queue_push(a, &b);
    b = 4;
    priority_queue_push(a, &b);
    b = 5;
    priority_queue_push(a, &b);
    b = 9;
    priority_queue_push(a, &b);
    b = 2;
    priority_queue_push(a, &b);
    b = 3;
    priority_queue_push(a, &b);
    b = -5;
    priority_queue_push(a, &b);
    b = 7;
    priority_queue_push(a, &b);
    b = 3;
    priority_queue_push(a, &b);
    b = 4;
    priority_queue_push(a, &b);
    b = 3;
    priority_queue_push(a, &b);
    b = 11;
    priority_queue_push(a, &b);
    b = 6;
    priority_queue_push(a, &b);
    b = 0xdeadbeef;
    priority_queue_front(&b, a);
    assert(b == 11);
    priority_queue_clear(a);
    assert(priority_queue_is_empty(a));
    b = 0xdeadbeef;
    assert(!priority_queue_front(&b, a));
    assert(b == 0xdeadbeef);
    assert(priority_queue_is_empty(a));
    a = priority_queue_destroy(a);
    assert(a == NULL);
}
