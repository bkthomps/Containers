#include "test.h"
#include "../src/include/queue.h"

static void test_invalid_init(void)
{
    assert(!queue_init(0));
}

static void test_linear_operations(queue me)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int stuff;
    int get;
    int i;
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
}

static void test_array_copy(queue me)
{
    int get_arr[10] = {0};
    int get;
    int i;
    queue_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        assert(get_arr[i] == i + 1);
    }
    for (i = 0; i < 9; i++) {
        get = 0;
        assert(queue_pop(&get, me));
        assert(get == i + 1);
    }
}

static void test_array_trim(queue me)
{
    int get;
    queue_trim(me);
    assert(queue_size(me) == 1);
    queue_clear(me);
    assert(queue_size(me) == 0);
    get = 0;
    assert(!queue_pop(&get, me));
    assert(!queue_front(&get, me));
    assert(!queue_back(&get, me));
}

static void test_basic(void)
{
    queue me = queue_init(sizeof(int));
    assert(me);
    test_linear_operations(me);
    test_array_copy(me);
    test_array_trim(me);
    assert(!queue_destroy(me));
}

static void test_large_alloc(void)
{
    int old_size;
    int pop_count;
    int get;
    int i;
    queue me = queue_init(sizeof(int));
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
    assert(!queue_destroy(me));
}

static void test_automated_trim(void)
{
    queue me = queue_init(sizeof(int));
    int get;
    int i;
    for (i = 0; i < 100; i++) {
        queue_push(me, &i);
        queue_pop(&get, me);
    }
    assert(!queue_destroy(me));
}

#if STUB_MALLOC
static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!queue_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(!queue_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 2;
    assert(!queue_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 3;
    assert(!queue_init(sizeof(int)));
}
#endif

struct pair {
    int cur_node;
    int cur_cost;
};

static int test_puzzle(int start_node, int dest_node)
{
    queue q = queue_init(sizeof(struct pair));
    struct pair cur;
    cur.cur_node = start_node;
    cur.cur_cost = 0;
    assert(queue_is_empty(q));
    queue_push(q, &cur);
    assert(queue_size(q) == 1);
    while (!queue_is_empty(q)) {
        int node;
        int cost;
        queue_pop(&cur, q);
        node = cur.cur_node;
        cost = cur.cur_cost;
        if (node > 2 * dest_node || node < 1) {
            continue;
        }
        if (node == dest_node) {
            queue_destroy(q);
            return cost;
        }
        cur.cur_cost = cost + 1;
        cur.cur_node = node - 1;
        queue_push(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == node - 1);
        cur.cur_node = 2 * node;
        queue_push(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == 2 * node);
    }
    queue_destroy(q);
    return -1;
}

void test_queue(void)
{
    test_invalid_init();
    test_basic();
    test_large_alloc();
    test_automated_trim();
#if STUB_MALLOC
    test_init_out_of_memory();
#endif
    assert(test_puzzle(2, 5) == 4);
    assert(test_puzzle(2, 10) == 5);
}
