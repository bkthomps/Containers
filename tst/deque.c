#include "test.h"
#include "../src/deque.h"

static void test_invalid_init(void)
{
    assert(!deque_init(0));
}

static void test_copy(deque me)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get_arr[10] = {0};
    int trimmed[5] = {0};
    int get;
    int i;
    for (i = 0; i < 10; i++) {
        deque_push_front(me, &val[i]);
        get = 0;
        deque_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(deque_size(me) == 10);
    assert(!deque_is_empty(me));
    deque_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        get = 0;
        deque_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    get = 5;
    deque_trim(me);
    for (i = 0; i < 7; i++) {
        deque_pop_back(&get, me);
        assert(get == val[i]);
    }
    deque_copy_to_array(trimmed, me);
    assert(deque_size(me) == 3);
    for (i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
}

static void test_linear_operations(deque me)
{
    int arr[3] = {0};
    int get;
    int set;
    int add = 3;
    deque_push_back(me, &add);
    add = -2;
    deque_push_back(me, &add);
    add = -1;
    deque_push_back(me, &add);
    assert(deque_size(me) == 6);
    get = 0xdeadbeef;
    deque_get_first(&get, me);
    assert(get == 10);
    get = 0xdeadbeef;
    deque_get_at(&get, me, 0);
    assert(get == 10);
    deque_get_at(&get, me, 1);
    assert(get == 9);
    deque_get_at(&get, me, 2);
    assert(get == 8);
    deque_get_at(&get, me, 3);
    assert(get == 3);
    deque_get_at(&get, me, 4);
    assert(get == -2);
    deque_get_at(&get, me, 5);
    assert(get == -1);
    get = 0xdeadbeef;
    deque_get_last(&get, me);
    assert(get == -1);
    deque_pop_front(&get, me);
    assert(get == 10);
    deque_pop_back(&get, me);
    assert(get == -1);
    deque_pop_back(&get, me);
    assert(get == -2);
    assert(deque_size(me) == 3);
    get = 345;
    deque_get_first(&get, me);
    assert(get == 9);
    deque_get_at(&get, me, 1);
    assert(get == 8);
    deque_get_last(&get, me);
    assert(get == 3);
    set = 12;
    deque_set_first(me, &set);
    set = 13;
    deque_set_at(me, 1, &set);
    set = 14;
    deque_set_last(me, &set);
    deque_copy_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    deque_set_at(me, 0, &set);
    set = -6;
    deque_set_at(me, 1, &set);
    set = -7;
    deque_set_at(me, 2, &set);
    deque_copy_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
}

static void test_invalid_input(deque me)
{
    int set;
    assert(deque_set_at(me, 4, &set) == -EINVAL);
    assert(deque_get_at(&set, me, 4) == -EINVAL);
    assert(deque_set_at(me, -1, &set) == -EINVAL);
    assert(deque_get_at(&set, me, -1) == -EINVAL);
}

static void test_basic(void)
{
    deque me = deque_init(sizeof(int));
    assert(me);
    assert(deque_size(me) == 0);
    assert(deque_is_empty(me));
    test_copy(me);
    test_linear_operations(me);
    test_invalid_input(me);
    deque_clear(me);
    assert(deque_size(me) == 0);
    assert(deque_is_empty(me));
    assert(!deque_destroy(me));
}

static void test_trim(void)
{
    deque me = deque_init(sizeof(int));
    int i;
    for (i = 0; i < 100; i++) {
        int get;
        deque_push_back(me, &i);
        deque_pop_front(&get, me);
    }
    deque_destroy(me);
}

static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!deque_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(!deque_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 2;
    assert(!deque_init(sizeof(int)));
}

static void test_trim_out_of_memory(void)
{
    deque me = deque_init(sizeof(int));
    int i;
    for (i = 0; i < 32; i++) {
        deque_push_back(me, &i);
    }
    assert(deque_size(me) == 32);
    fail_malloc = 1;
    assert(deque_trim(me) == -ENOMEM);
    for (i = 0; i < 32; i++) {
        int get = 0xdeadbeef;
        deque_get_at(&get, me, i);
        assert(get == i);
    }
    assert(deque_size(me) == 32);
    assert(!deque_destroy(me));
}

static void test_push_front_out_of_memory(void)
{
    /* TODO */
}

static void test_push_back_out_of_memory(void)
{
    /* TODO */
}

static void test_clear_out_of_memory(void)
{
    deque me = deque_init(sizeof(int));
    int i;
    for (i = 0; i < 32; i++) {
        deque_push_back(me, &i);
    }
    assert(deque_size(me) == 32);
    fail_malloc = 1;
    assert(deque_clear(me) == -ENOMEM);
    for (i = 0; i < 32; i++) {
        int get = 0xdeadbeef;
        deque_get_at(&get, me, i);
        assert(get == i);
    }
    assert(deque_size(me) == 32);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(deque_clear(me) == -ENOMEM);
    for (i = 0; i < 32; i++) {
        int get = 0xdeadbeef;
        deque_get_at(&get, me, i);
        assert(get == i);
    }
    assert(deque_size(me) == 32);
    assert(!deque_destroy(me));
}

void test_deque(void)
{
    test_invalid_init();
    test_basic();
    test_trim();
    test_init_out_of_memory();
    test_trim_out_of_memory();
    test_push_front_out_of_memory();
    test_push_back_out_of_memory();
    test_clear_out_of_memory();
}
