#include "test.h"
#include "../src/stack.h"

void test_stack(void)
{
    assert(!stack_init(0));
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    stack me = stack_init(sizeof(int));
    assert(me);
    assert(stack_size(me) == 0);
    assert(stack_is_empty(me));
    for (int i = 0; i < 10; i++) {
        stack_push(me, &val[i]);
        int get = 0;
        assert(stack_top(&get, me));
        assert(get == val[i]);
    }
    assert(stack_size(me) == 10);
    assert(!stack_is_empty(me));
    int get_arr[10] = {0};
    stack_copy_to_array(get_arr, me);
    for (int i = 0; i < 10; i++) {
        assert(get_arr[i] == i + 1);
    }
    for (int i = 0; i < 9; i++) {
        int get = 0;
        assert(stack_pop(&get, me));
        assert(get == 10 - i);
    }
    stack_trim(me);
    assert(stack_size(me) == 1);
    stack_clear(me);
    assert(stack_size(me) == 0);
    int get = 0;
    assert(!stack_pop(&get, me));
    assert(!stack_top(&get, me));
    me = stack_destroy(me);
    assert(!me);
    // Testing automatic trim
    me = stack_init(sizeof(int));
    for (int i = 0; i < 100; i++) {
        stack_push(me, &i);
        stack_pop(&get, me);
    }
    stack_destroy(me);
}
