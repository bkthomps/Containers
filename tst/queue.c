#include "test.h"
#include "../src/queue.h"

void test_queue(void)
{
    assert(!queue_init(0));
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    queue me = queue_init(sizeof(int));
    assert(me);
    assert(queue_size(me) == 0);
    assert(queue_is_empty(me));
    for (int i = 0; i < 10; i++) {
        queue_push(me, &val[i]);
        int get = 0;
        assert(queue_back(&get, me));
        assert(get == val[i]);
        int stuff = 0;
        assert(queue_front(&stuff, me));
        assert(stuff == 1);
    }
    assert(queue_size(me) == 10);
    assert(!queue_is_empty(me));
    int get_arr[10] = {0};
    queue_copy_to_array(get_arr, me);
    for (int i = 0; i < 10; i++) {
        assert(get_arr[i] == i + 1);
    }
    for (int i = 0; i < 9; i++) {
        int get = 0;
        assert(queue_pop(&get, me));
        assert(get == i + 1);
    }
    queue_trim(me);
    assert(queue_size(me) == 1);
    queue_clear(me);
    assert(queue_size(me) == 0);
    int get = 0;
    assert(!queue_pop(&get, me));
    assert(!queue_front(&get, me));
    assert(!queue_back(&get, me));
    me = queue_destroy(me);
    assert(!me);
    me = queue_init(sizeof(int));
    assert(me);
    for (int i = 123; i < 123456; i++) {
        queue_push(me, &i);
    }
    const int old_size = queue_size(me);
    int pop_count = 0;
    while (!queue_is_empty(me)) {
        queue_pop(&get, me);
        pop_count++;
    }
    assert(pop_count == old_size);
    queue_destroy(me);
    // Testing automatic trim
    me = queue_init(sizeof(int));
    for (int i = 0; i < 100; i++) {
        queue_push(me, &i);
        queue_pop(&get, me);
    }
    queue_destroy(me);
}
