#include "test.h"
#include "../src/include/vector.h"
#include "../src/include/priority_queue.h"

/*
 * Include this for the stubs.
 */
struct internal_priority_queue {
    size_t data_size;
    int (*comparator)(const void *const one, const void *const two);
    vector data;
};

static void priority_queue_verify(priority_queue me)
{
    int i;
    void *const vector_storage = vector_get_data(me->data);
    const int size = vector_size(me->data);
    for (i = 0; i < size; i++) {
        const int val = *(int *) ((char *) vector_storage + i * me->data_size);
        const int left_child = 2 * i + 1;
        const int right_child = 2 * i + 2;
        if (left_child < size) {
            void *left_data =
                    (char *) vector_storage + left_child * me->data_size;
            const int left_val = *(int *) left_data;
            assert(val >= left_val);
        }
        if (right_child < size) {
            void *right_data =
                    (char *) vector_storage + right_child * me->data_size;
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

static int stub_priority_queue_push(priority_queue me, void *const data)
{
    const int ret = priority_queue_push(me, data);
    priority_queue_verify(me);
    return ret;
}

static int stub_priority_queue_pop(void *const data, priority_queue me)
{
    const int ret = priority_queue_pop(data, me);
    priority_queue_verify(me);
    return ret;
}

static void test_invalid_init(void)
{
    assert(!priority_queue_init(0, compare_int));
    assert(!priority_queue_init(sizeof(int), NULL));
}

static void test_basic(void)
{
    int item;
    int latest;
    size_t i;
    priority_queue me = priority_queue_init(sizeof(int), compare_int);
    assert(me);
    assert(priority_queue_size(me) == 0);
    assert(priority_queue_is_empty(me));
    item = 0xfacade;
    assert(!priority_queue_pop(&item, me));
    assert(item == 0xfacade);
    item = 5;
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
    item = 0xfacade;
    priority_queue_front(&item, me);
    assert(item == 11);
    item = 0xfacade;
    stub_priority_queue_pop(&item, me);
    assert(item == 11);
    assert(priority_queue_size(me) == 15);
    item = 0xfacade;
    priority_queue_front(&item, me);
    assert(item == 9);
    latest = item;
    for (i = 0; i < 15; i++) {
        stub_priority_queue_pop(&item, me);
        assert(item <= latest);
        latest = item;
        assert(priority_queue_size(me) == 15 - i - 1);
    }
    priority_queue_clear(me);
    assert(priority_queue_is_empty(me));
    item = 0xfacade;
    assert(!priority_queue_front(&item, me));
    assert(item == 0xfacade);
    assert(priority_queue_is_empty(me));
    assert(!priority_queue_destroy(me));
}

#if STUB_MALLOC
static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!priority_queue_init(sizeof(int), compare_int));
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(!priority_queue_init(sizeof(int), compare_int));
    fail_malloc = 1;
    delay_fail_malloc = 2;
    assert(!priority_queue_init(sizeof(int), compare_int));
}
#endif

#if STUB_MALLOC
static void test_push_out_of_memory(void)
{
    int i;
    int get = 0xfacade;
    priority_queue me = priority_queue_init(sizeof(int), compare_int);
    for (i = 0; i < 16; i++) {
        assert(priority_queue_push(me, &i) == 0);
    }
    assert(priority_queue_size(me) == 16);
    fail_malloc = 1;
    assert(priority_queue_push(me, &get) == -ENOMEM);
    for (i = 0; i < 16; i++) {
        get = 0xfacade;
        assert(priority_queue_pop(&get, me));
        assert(get == 15 - i);
    }
    assert(priority_queue_size(me) == 0);
    priority_queue_clear(me);
    for (i = 0; i < 12; i++) {
        assert(priority_queue_push(me, &i) == 0);
    }
    assert(priority_queue_size(me) == 12);
    fail_realloc = 1;
    assert(priority_queue_push(me, &get) == -ENOMEM);
    for (i = 0; i < 12; i++) {
        get = 0xfacade;
        assert(priority_queue_pop(&get, me));
        assert(get == 11 - i);
    }
    assert(!priority_queue_destroy(me));
}
#endif

struct big_object {
    int n;
    double d;
    signed char c[8];
};

static int compare_big_object(const void *const one, const void *const two)
{
    const struct big_object *const a = one;
    const struct big_object *const b = two;
    /* Bigger number = lower priority */
    return b->n - a->n;
}

static void test_big_object(void)
{
    int i;
    priority_queue me = priority_queue_init(sizeof(struct big_object),
                                            compare_big_object);
    assert(me);
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        b.n = INT_MIN + i;
        b.d = i + 0.5;
        for (j = 0; j < 8; j++) {
            b.c[j] = (signed char) (SCHAR_MIN + i + j);
        }
        assert(priority_queue_push(me, &b) == 0);
        b.n = -1;
        b.d = -1;
        for (j = 0; j < 8; j++) {
            b.c[j] = -1;
        }
    }
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        assert(priority_queue_pop(&b, me) == 1);
        assert(b.n == INT_MIN + i);
        assert(b.d == i + 0.5);
        for (j = 0; j < 8; j++) {
            assert(b.c[j] == SCHAR_MIN + i + j);
        }
    }
    assert(!priority_queue_destroy(me));
}

void test_priority_queue(void)
{
    test_invalid_init();
    test_basic();
#if STUB_MALLOC
    test_init_out_of_memory();
    test_push_out_of_memory();
#endif
    test_big_object();
    priority_queue_destroy(NULL);
}
