#include "test.h"
#include "../src/include/stack.h"

static void test_invalid_init(void)
{
    assert(!stack_init(0));
}

static void test_linear_operations(stack me)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get;
    int i;
    assert(stack_size(me) == 0);
    assert(stack_is_empty(me));
    for (i = 0; i < 10; i++) {
        stack_push(me, &val[i]);
        get = 0;
        assert(stack_top(&get, me));
        assert(get == val[i]);
    }
}

static void test_array_copy(stack me)
{
    int get_arr[10] = {0};
    int get;
    int i;
    assert(stack_size(me) == 10);
    assert(!stack_is_empty(me));
    stack_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        assert(get_arr[i] == i + 1);
    }
    for (i = 0; i < 9; i++) {
        get = 0;
        assert(stack_pop(&get, me));
        assert(get == 10 - i);
    }
    stack_trim(me);
    assert(stack_size(me) == 1);
    stack_clear(me);
    assert(stack_size(me) == 0);
    get = 0;
    assert(!stack_pop(&get, me));
    assert(!stack_top(&get, me));
}

static void test_basic(void)
{
    stack me = stack_init(sizeof(int));
    assert(me);
    test_linear_operations(me);
    test_array_copy(me);
    assert(!stack_destroy(me));
}

static void test_automated_trim(void)
{
    stack me = stack_init(sizeof(int));
    int get;
    int i;
    for (i = 0; i < 100; i++) {
        stack_push(me, &i);
        stack_pop(&get, me);
    }
    assert(!stack_destroy(me));
}

static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!stack_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(!stack_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 2;
    assert(!stack_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 3;
    assert(!stack_init(sizeof(int)));
}

void test_stack(void)
{
    test_invalid_init();
    test_basic();
    test_automated_trim();
    test_init_out_of_memory();
}
