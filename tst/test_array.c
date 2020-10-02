#include "test.h"
#include "../src/include/array.h"

static void test_invalid_init(void)
{
    const size_t max_size = -1;
    assert(!array_init(1, 0));
    assert(!array_init(max_size, max_size));
    assert(!array_init(1, max_size));
}

static void test_empty_array(void)
{
    int get = 0xfacade;
    int arr[2] = {0xfacade, 0xfacade};
    array me = array_init(0, sizeof(int));
    assert(me);
    assert(array_size(me) == 0);
    array_copy_to_array(arr, me);
    assert(arr[0] == 0xfacade);
    assert(arr[1] == 0xfacade);
    assert(!array_get_data(me));
    assert(array_set(me, 0, &get) == -EINVAL);
    assert(array_get(&get, me, 0) == -EINVAL);
    assert(!array_destroy(me));
}

static void test_individual_operations(array me)
{
    int i;
    for (i = 0; i < 10; i++) {
        int get = 0xfacade;
        array_get(&get, me, i);
        assert(get == 0);
    }
    for (i = 0; i < 10; i++) {
        int get = 0xfacade;
        array_set(me, i, &i);
        array_get(&get, me, i);
        assert(get == i);
    }
    for (i = 0; i < 10; i++) {
        int get = 0xfacade;
        array_get(&get, me, i);
        assert(get == i);
    }
}

static void test_array_copying(array me)
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

static void test_out_of_bounds(array me)
{
    int get = 0xfacade;
    assert(array_set(me, -1, &get) == -EINVAL);
    assert(array_get(&get, me, -1) == -EINVAL);
}

static void test_not_empty_array(void)
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

#if STUB_MALLOC
static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!array_init(10, sizeof(int)));
}
#endif

struct big_object {
    int n;
    double d;
    signed char c[8];
};

static void test_big_object(void)
{
    int i;
    array me = array_init(16, sizeof(struct big_object));
    assert(me);
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        b.n = INT_MIN + i;
        b.d = i + 0.5;
        for (j = 0; j < 8; j++) {
            b.c[j] = (signed char) (SCHAR_MIN + i + j);
        }
        assert(array_set(me, i, &b) == 0);
        b.n = -1;
        b.d = -1;
        for (j = 0; j < 8; j++) {
            b.c[j] = -1;
        }
    }
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        assert(array_get(&b, me, i) == 0);
        assert(b.n == INT_MIN + i);
        assert(b.d == i + 0.5);
        for (j = 0; j < 8; j++) {
            assert(b.c[j] == SCHAR_MIN + i + j);
        }
    }
    assert(!array_destroy(me));
}

static void test_add_all(void)
{
    int get;
    size_t i;
    int arr_1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int arr_2[] = {100, 200, 300, 400, 500, 600, 700, 800, 900};
    array me = array_init(10, sizeof(int));
    assert(array_add_all(me, arr_1, 11) == -BK_EINVAL);
    assert(array_add_all(me, arr_1, 10) == BK_OK);
    for (i = 0; i < sizeof(arr_1) / sizeof(*arr_1); i++) {
        array_get(&get, me, i);
        assert(get == arr_1[i]);
    }
    assert(array_add_all(me, arr_2, 9) == BK_OK);
    for (i = 0; i < sizeof(arr_2) / sizeof(*arr_2); i++) {
        array_get(&get, me, i);
        assert(get == arr_2[i]);
    }
    array_get(&get, me, 9);
    assert(get == 10);
    array_destroy(me);
}

void test_array(void)
{
    test_invalid_init();
    test_empty_array();
    test_not_empty_array();
#if STUB_MALLOC
    test_init_out_of_memory();
#endif
    test_big_object();
    test_add_all();
    array_destroy(NULL);
}
