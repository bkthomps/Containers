#include "test.h"
#include "../src/array.h"

void test_invalid_init(void)
{
    assert(!array_init(-1, sizeof(int)));
    assert(!array_init(1, 0));
}

void test_empty_array(void)
{
    int get = 0xdeadbeef;
    int arr[2] = {0xdeadbeef, 0xdeadbeef};
    array me = array_init(0, sizeof(int));
    assert(me);
    assert(array_size(me) == 0);
    array_copy_to_array(arr, me);
    assert(arr[0] == 0xdeadbeef);
    assert(arr[1] == 0xdeadbeef);
    assert(!array_get_data(me));
    assert(array_set(me, 0, &get) == -EINVAL);
    assert(array_get(&get, me, 0) == -EINVAL);
    assert(!array_destroy(me));
}

void test_individual_operations(array me)
{
    int i;
    for (i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_get(&get, me, i);
        assert(get == 0);
    }
    for (i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_set(me, i, &i);
        array_get(&get, me, i);
        assert(get == i);
    }
    for (i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_get(&get, me, i);
        assert(get == i);
    }
}

void test_array_copying(array me)
{
    int i;
    int *data;
    int arr[10] = {0};
    array_copy_to_array(arr, me);
    for (i = 0; i < 10; i++) {
        assert(arr[i] == i);
    }
    data = array_get_data(me);
    for (i = 0; i < 10; i++) {
        assert(data[i] == i);
    }
}

void test_out_of_bounds(array me)
{
    int get = 0xdeadbeef;
    assert(array_set(me, -1, &get) == -EINVAL);
    assert(array_get(&get, me, -1) == -EINVAL);
}

void test_not_empty_array(void)
{
    array me = array_init(10, sizeof(int));
    assert(me);
    assert(array_size(me) == 10);
    test_individual_operations(me);
    test_array_copying(me);
    test_out_of_bounds(me);
    me = array_destroy(me);
    assert(!me);
}

void test_init_fail_malloc(void)
{
    fail_malloc = 1;
    assert(!array_init(10, sizeof(int)));
}

void test_init_fail_calloc(void)
{
    fail_calloc = 1;
    assert(!array_init(10, sizeof(int)));
}

void test_array(void)
{
    test_invalid_init();
    test_empty_array();
    test_not_empty_array();
    test_init_fail_malloc();
    test_init_fail_calloc();
}
