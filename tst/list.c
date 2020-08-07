#include "test.h"
#include "../src/include/list.h"

static void test_invalid_init(void)
{
    assert(!list_init(0));
}

static void test_front(list me)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get_arr[10] = {0};
    int get;
    int i;
    assert(list_size(me) == 0);
    assert(list_is_empty(me));
    for (i = 0; i < 10; i++) {
        list_add_first(me, &val[i]);
        assert(list_size(me) == i + 1);
        get = 0;
        list_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(list_size(me) == 10);
    assert(!list_is_empty(me));
    list_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        get = 0;
        list_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    for (i = 0; i < 7; i++) {
        list_remove_last(me);
    }
}

static void test_linear_operations(list me)
{
    int trimmed[5] = {0};
    int index;
    int first;
    int add;
    int get;
    int set;
    int i;
    list_copy_to_array(trimmed, me);
    assert(list_size(me) == 3);
    for (i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
    index = list_size(me);
    add = 3;
    list_add_at(me, index, &add);
    add = -1;
    list_add_at(me, 1, &add);
    add = -2;
    list_add_last(me, &add);
    assert(list_size(me) == 6);
    get = 0xfacade;
    list_get_first(&get, me);
    assert(get == 10);
    get = 0xfacade;
    list_get_at(&get, me, 0);
    assert(get == 10);
    list_get_at(&get, me, 1);
    assert(get == -1);
    list_get_at(&get, me, 2);
    assert(get == 9);
    list_get_at(&get, me, 3);
    assert(get == 8);
    list_get_at(&get, me, 4);
    assert(get == 3);
    list_get_at(&get, me, 5);
    assert(get == -2);
    get = 0xfacade;
    list_get_last(&get, me);
    assert(get == -2);
    list_remove_first(me);
    list_remove_at(me, 2);
    list_remove_last(me);
    get = 34;
    list_add_at(me, 0, &get);
    first = 0xfacade;
    list_get_first(&first, me);
    assert(first == get);
    list_remove_first(me);
    assert(list_size(me) == 3);
    get = 345;
    list_get_first(&get, me);
    assert(get == -1);
    list_get_at(&get, me, 1);
    assert(get == 9);
    list_get_last(&get, me);
    assert(get == 3);
    set = 12;
    list_set_first(me, &set);
    set = 13;
    list_set_at(me, 1, &set);
    set = 14;
    list_set_last(me, &set);
}

static void test_array_copy(list me)
{
    int arr[3] = {0};
    int set;
    list_copy_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    list_set_at(me, 0, &set);
    set = -6;
    list_set_at(me, 1, &set);
    set = -7;
    list_set_at(me, 2, &set);
    list_copy_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
}

static void test_invalid_index(list me)
{
    int set = 0xfacade;
    assert(list_set_at(me, 4, &set) == -EINVAL);
    assert(list_get_at(&set, me, 4) == -EINVAL);
    assert(list_remove_at(me, 4) == -EINVAL);
    assert(list_add_at(me, 5, &set) == -EINVAL);
    assert(list_set_at(me, -1, &set) == -EINVAL);
    assert(list_get_at(&set, me, -1) == -EINVAL);
    assert(list_remove_at(me, -1) == -EINVAL);
    assert(list_add_at(me, -1, &set) == -EINVAL);
    list_clear(me);
    assert(list_size(me) == 0);
    assert(list_is_empty(me));
    assert(list_remove_first(me) == -EINVAL);
    assert(list_remove_last(me) == -EINVAL);
}

static void test_basic(void)
{
    list me = list_init(sizeof(int));
    assert(me);
    test_front(me);
    test_linear_operations(me);
    test_array_copy(me);
    test_invalid_index(me);
    assert(!list_destroy(me));
}

static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!list_init(sizeof(int)));
}

static void test_add_first_out_of_memory(void)
{
    int i;
    list me = list_init(sizeof(int));
    assert(me);
    for (i = 0; i < 16; i++) {
        assert(list_add_first(me, &i) == 0);
    }
    assert(list_size(me) == 16);
    fail_malloc = 1;
    assert(list_add_first(me, &i) == -ENOMEM);
    assert(list_size(me) == 16);
    for (i = 0; i < 16; i++) {
        int get = 0xfacade;
        assert(list_get_at(&get, me, i) == 0);
        assert(get == 15 - i);
    }
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(list_add_first(me, &i) == -ENOMEM);
    assert(list_size(me) == 16);
    for (i = 0; i < 16; i++) {
        int get = 0xfacade;
        assert(list_get_at(&get, me, i) == 0);
        assert(get == 15 - i);
    }
    assert(!list_destroy(me));
}

static void test_add_at_out_of_memory(void)
{
    int get;
    int i;
    list me = list_init(sizeof(int));
    assert(me);
    i = 982;
    assert(list_add_first(me, &i) == 0);
    i = 157;
    assert(list_add_first(me, &i) == 0);
    for (i = 1; i < 15; i++) {
        assert(list_add_at(me, i, &i) == 0);
    }
    assert(list_size(me) == 16);
    fail_malloc = 1;
    assert(list_add_at(me, 5, &i) == -ENOMEM);
    assert(list_size(me) == 16);
    get = 0xfacade;
    assert(list_get_at(&get, me, 0) == 0);
    assert(get == 157);
    for (i = 1; i < 15; i++) {
        get = 0xfacade;
        assert(list_get_at(&get, me, i) == 0);
        assert(get == i);
    }
    get = 0xfacade;
    assert(list_get_at(&get, me, 15) == 0);
    assert(get == 982);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(list_add_at(me, 5, &i) == -ENOMEM);
    assert(list_size(me) == 16);
    get = 0xfacade;
    assert(list_get_at(&get, me, 0) == 0);
    assert(get == 157);
    for (i = 1; i < 15; i++) {
        get = 0xfacade;
        assert(list_get_at(&get, me, i) == 0);
        assert(get == i);
    }
    get = 0xfacade;
    assert(list_get_at(&get, me, 15) == 0);
    assert(get == 982);
    assert(!list_destroy(me));
}

static void test_add_last_out_of_memory(void)
{
    int i;
    list me = list_init(sizeof(int));
    assert(me);
    for (i = 0; i < 16; i++) {
        assert(list_add_last(me, &i) == 0);
    }
    assert(list_size(me) == 16);
    fail_malloc = 1;
    assert(list_add_last(me, &i) == -ENOMEM);
    assert(list_size(me) == 16);
    for (i = 0; i < 16; i++) {
        int get = 0xfacade;
        assert(list_get_at(&get, me, i) == 0);
        assert(get == i);
    }
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(list_add_last(me, &i) == -ENOMEM);
    assert(list_size(me) == 16);
    for (i = 0; i < 16; i++) {
        int get = 0xfacade;
        assert(list_get_at(&get, me, i) == 0);
        assert(get == i);
    }
    assert(!list_destroy(me));
}

void test_remove_all(void)
{
    int i;
    list me = list_init(sizeof(int));
    for (i = 0; i < 100; i++) {
        list_add_first(me, &i);
        assert(list_size(me) == i + 1);
    }
    for (i = 0; i < 100; i++) {
        list_remove_first(me);
        assert(list_size(me) == 100 - i - 1);
    }
    assert(list_is_empty(me));
    for (i = 0; i < 100; i++) {
        list_add_first(me, &i);
        assert(list_size(me) == i + 1);
    }
    assert(list_size(me) == 100);
    assert(!list_destroy(me));
}

struct pair {
    int cur_node;
    int cur_cost;
};

static int test_puzzle(int start_node, int dest_node)
{
    list q = list_init(sizeof(struct pair));
    struct pair cur;
    cur.cur_node = start_node;
    cur.cur_cost = 0;
    assert(list_is_empty(q));
    list_add_last(q, &cur);
    assert(list_size(q) == 1);
    while (!list_is_empty(q)) {
        int node;
        int cost;
        list_get_first(&cur, q);
        list_remove_first(q);
        node = cur.cur_node;
        cost = cur.cur_cost;
        if (node > 2 * dest_node || node < 1) {
            continue;
        }
        if (node == dest_node) {
            list_destroy(q);
            return cost;
        }
        cur.cur_cost = cost + 1;
        cur.cur_node = node - 1;
        list_add_last(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == node - 1);
        cur.cur_node = 2 * node;
        list_add_last(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == 2 * node);
    }
    list_destroy(q);
    return -1;
}

void test_list(void)
{
    test_invalid_init();
    test_basic();
    test_init_out_of_memory();
    test_add_first_out_of_memory();
    test_add_at_out_of_memory();
    test_add_last_out_of_memory();
    test_remove_all();
    assert(test_puzzle(2, 5) == 4);
    assert(test_puzzle(2, 10) == 5);
}
