#include "test.h"
#include "../src/queue.h"

void test_queue(void)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get_arr[10] = {0};
    queue me;
    int get;
    int stuff;
    int old_size;
    int pop_count;
    int i;
    assert(!queue_init(0));
    me = queue_init(sizeof(int));
    assert(me);
    assert(queue_size(me) == 0);
    assert(queue_is_empty(me));
    for (i = 0; i < 10; i++) {
        queue_push(me, &val[i]);
        get = 0;
        assert(queue_back(&get, me));
        assert(get == val[i]);
        stuff = 0;
        assert(queue_front(&stuff, me));
        assert(stuff == 1);
    }
    assert(queue_size(me) == 10);
    assert(!queue_is_empty(me));
    queue_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        assert(get_arr[i] == i + 1);
    }
    for (i = 0; i < 9; i++) {
        get = 0;
        assert(queue_pop(&get, me));
        assert(get == i + 1);
    }
    queue_trim(me);
    assert(queue_size(me) == 1);
    queue_clear(me);
    assert(queue_size(me) == 0);
    get = 0;
    assert(!queue_pop(&get, me));
    assert(!queue_front(&get, me));
    assert(!queue_back(&get, me));
    me = queue_destroy(me);
    assert(!me);
    me = queue_init(sizeof(int));
    assert(me);
    for (i = 123; i < 123456; i++) {
        queue_push(me, &i);
    }
    old_size = queue_size(me);
    pop_count = 0;
    while (!queue_is_empty(me)) {
        queue_pop(&get, me);
        pop_count++;
    }
    assert(pop_count == old_size);
    queue_destroy(me);
    // Testing automatic trim
    me = queue_init(sizeof(int));
    for (i = 0; i < 100; i++) {
        queue_push(me, &i);
        queue_pop(&get, me);
    }
    queue_destroy(me);
}
