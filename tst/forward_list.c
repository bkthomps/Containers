#include "test.h"
#include "../src/include/forward_list.h"

static void test_invalid_init(void)
{
    assert(!forward_list_init(0));
}

static void test_front(forward_list me)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get_arr[10] = {0};
    int get;
    int i;
    assert(forward_list_size(me) == 0);
    assert(forward_list_is_empty(me));
    for (i = 0; i < 10; i++) {
        forward_list_add_first(me, &val[i]);
        assert(forward_list_size(me) == i + 1);
        get = 0;
        forward_list_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(forward_list_size(me) == 10);
    assert(!forward_list_is_empty(me));
    forward_list_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        get = 0;
        forward_list_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    for (i = 0; i < 7; i++) {
        forward_list_remove_last(me);
    }
}

static void test_linear_operations(forward_list me)
{
    int trimmed[5] = {0};
    int index;
    int first;
    int add;
    int get;
    int set;
    int i;
    forward_list_copy_to_array(trimmed, me);
    assert(forward_list_size(me) == 3);
    for (i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
    index = forward_list_size(me);
    add = 3;
    forward_list_add_at(me, index, &add);
    add = -1;
    forward_list_add_at(me, 1, &add);
    add = -2;
    forward_list_add_last(me, &add);
    assert(forward_list_size(me) == 6);
    get = 0xfacade;
    forward_list_get_first(&get, me);
    assert(get == 10);
    get = 0xfacade;
    forward_list_get_at(&get, me, 0);
    assert(get == 10);
    forward_list_get_at(&get, me, 1);
    assert(get == -1);
    forward_list_get_at(&get, me, 2);
    assert(get == 9);
    forward_list_get_at(&get, me, 3);
    assert(get == 8);
    forward_list_get_at(&get, me, 4);
    assert(get == 3);
    forward_list_get_at(&get, me, 5);
    assert(get == -2);
    get = 0xfacade;
    forward_list_get_last(&get, me);
    assert(get == -2);
    forward_list_remove_first(me);
    forward_list_remove_at(me, 2);
    forward_list_remove_last(me);
    get = 34;
    forward_list_add_at(me, 0, &get);
    first = 0xfacade;
    forward_list_get_first(&first, me);
    assert(first == get);
    forward_list_remove_first(me);
    assert(forward_list_size(me) == 3);
    get = 345;
    forward_list_get_first(&get, me);
    assert(get == -1);
    forward_list_get_at(&get, me, 1);
    assert(get == 9);
    forward_list_get_last(&get, me);
    assert(get == 3);
    set = 12;
    forward_list_set_first(me, &set);
    set = 13;
    forward_list_set_at(me, 1, &set);
    set = 14;
    forward_list_set_last(me, &set);
}

static void test_array_copy(forward_list me)
{
    int arr[3] = {0};
    int set;
    forward_list_copy_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    forward_list_set_at(me, 0, &set);
    set = -6;
    forward_list_set_at(me, 1, &set);
    set = -7;
    forward_list_set_at(me, 2, &set);
    forward_list_copy_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
}

static void test_invalid_index(forward_list me)
{
    int set = 0xfacade;
    assert(forward_list_set_at(me, 4, &set) == -EINVAL);
    assert(forward_list_get_at(&set, me, 4) == -EINVAL);
    assert(forward_list_remove_at(me, 4) == -EINVAL);
    assert(forward_list_add_at(me, 5, &set) == -EINVAL);
    assert(forward_list_set_at(me, -1, &set) == -EINVAL);
    assert(forward_list_get_at(&set, me, -1) == -EINVAL);
    assert(forward_list_remove_at(me, -1) == -EINVAL);
    assert(forward_list_add_at(me, -1, &set) == -EINVAL);
    forward_list_clear(me);
    assert(forward_list_size(me) == 0);
    assert(forward_list_is_empty(me));
    assert(forward_list_remove_first(me) == -EINVAL);
    assert(forward_list_remove_last(me) == -EINVAL);
}

static void test_basic(void)
{
    forward_list me = forward_list_init(sizeof(int));
    assert(me);
    test_front(me);
    test_linear_operations(me);
    test_array_copy(me);
    test_invalid_index(me);
    assert(!forward_list_destroy(me));
}

static void test_add_back(void)
{
    int i;
    forward_list me = forward_list_init(sizeof(int));
    assert(me);
    for (i = 1; i < 10000; i++) {
        int get = 0xfacade;
        forward_list_add_last(me, &i);
        forward_list_get_last(&get, me);
        assert(get == i);
        if (i % 5 == 0) {
            forward_list_remove_last(me);
            forward_list_get_last(&get, me);
            assert(get == i - 1);
        }
    }
    assert(!forward_list_destroy(me));
}

static void test_init_out_of_memory(void)
{
#if STUB_MALLOC
    fail_malloc = 1;
    assert(!forward_list_init(sizeof(int)));
#endif
}

static void test_add_first_out_of_memory(void)
{
#if STUB_MALLOC
    int i;
    forward_list me = forward_list_init(sizeof(int));
    assert(me);
    for (i = 0; i < 16; i++) {
        assert(forward_list_add_first(me, &i) == 0);
    }
    assert(forward_list_size(me) == 16);
    fail_malloc = 1;
    assert(forward_list_add_first(me, &i) == -ENOMEM);
    assert(forward_list_size(me) == 16);
    for (i = 0; i < 16; i++) {
        int get = 0xfacade;
        assert(forward_list_get_at(&get, me, i) == 0);
        assert(get == 15 - i);
    }
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(forward_list_add_first(me, &i) == -ENOMEM);
    assert(forward_list_size(me) == 16);
    for (i = 0; i < 16; i++) {
        int get = 0xfacade;
        assert(forward_list_get_at(&get, me, i) == 0);
        assert(get == 15 - i);
    }
    assert(!forward_list_destroy(me));
#endif
}

static void test_add_at_out_of_memory(void)
{
#if STUB_MALLOC
    int get;
    int i;
    forward_list me = forward_list_init(sizeof(int));
    assert(me);
    i = 982;
    assert(forward_list_add_first(me, &i) == 0);
    i = 157;
    assert(forward_list_add_first(me, &i) == 0);
    for (i = 1; i < 15; i++) {
        assert(forward_list_add_at(me, i, &i) == 0);
    }
    assert(forward_list_size(me) == 16);
    fail_malloc = 1;
    assert(forward_list_add_at(me, 5, &i) == -ENOMEM);
    assert(forward_list_size(me) == 16);
    get = 0xfacade;
    assert(forward_list_get_at(&get, me, 0) == 0);
    assert(get == 157);
    for (i = 1; i < 15; i++) {
        get = 0xfacade;
        assert(forward_list_get_at(&get, me, i) == 0);
        assert(get == i);
    }
    get = 0xfacade;
    assert(forward_list_get_at(&get, me, 15) == 0);
    assert(get == 982);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(forward_list_add_at(me, 5, &i) == -ENOMEM);
    assert(forward_list_size(me) == 16);
    get = 0xfacade;
    assert(forward_list_get_at(&get, me, 0) == 0);
    assert(get == 157);
    for (i = 1; i < 15; i++) {
        get = 0xfacade;
        assert(forward_list_get_at(&get, me, i) == 0);
        assert(get == i);
    }
    get = 0xfacade;
    assert(forward_list_get_at(&get, me, 15) == 0);
    assert(get == 982);
    assert(!forward_list_destroy(me));
#endif
}

static void test_add_last_out_of_memory(void)
{
#if STUB_MALLOC
    int i;
    forward_list me = forward_list_init(sizeof(int));
    assert(me);
    for (i = 0; i < 16; i++) {
        assert(forward_list_add_last(me, &i) == 0);
    }
    assert(forward_list_size(me) == 16);
    fail_malloc = 1;
    assert(forward_list_add_last(me, &i) == -ENOMEM);
    assert(forward_list_size(me) == 16);
    for (i = 0; i < 16; i++) {
        int get = 0xfacade;
        assert(forward_list_get_at(&get, me, i) == 0);
        assert(get == i);
    }
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(forward_list_add_last(me, &i) == -ENOMEM);
    assert(forward_list_size(me) == 16);
    for (i = 0; i < 16; i++) {
        int get = 0xfacade;
        assert(forward_list_get_at(&get, me, i) == 0);
        assert(get == i);
    }
    assert(!forward_list_destroy(me));
#endif
}

void test_forward_list(void)
{
    test_invalid_init();
    test_basic();
    test_add_back();
    test_init_out_of_memory();
    test_add_first_out_of_memory();
    test_add_at_out_of_memory();
    test_add_last_out_of_memory();
}
