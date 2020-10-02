#include "test.h"
#include "../src/include/forward_list.h"

static void test_invalid_init(void)
{
    const size_t max_size = -1;
    assert(!forward_list_init(0));
    assert(!forward_list_init(max_size));
}

static void test_front(forward_list me)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get_arr[10] = {0};
    int get;
    size_t i;
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

#if STUB_MALLOC
static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!forward_list_init(sizeof(int)));
}
#endif

#if STUB_MALLOC
static void test_add_first_out_of_memory(void)
{
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
    assert(!forward_list_destroy(me));
}
#endif

#if STUB_MALLOC
static void test_add_at_out_of_memory(void)
{
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
}
#endif

#if STUB_MALLOC
static void test_add_last_out_of_memory(void)
{
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
    assert(!forward_list_destroy(me));
}
#endif

struct pair {
    int cur_node;
    int cur_cost;
};

static int test_puzzle_forwards(int start_node, int dest_node)
{
    forward_list q = forward_list_init(sizeof(struct pair));
    struct pair cur;
    cur.cur_node = start_node;
    cur.cur_cost = 0;
    assert(forward_list_is_empty(q));
    forward_list_add_last(q, &cur);
    assert(forward_list_size(q) == 1);
    while (!forward_list_is_empty(q)) {
        int node;
        int cost;
        forward_list_get_first(&cur, q);
        forward_list_remove_first(q);
        node = cur.cur_node;
        cost = cur.cur_cost;
        if (node > 2 * dest_node || node < 1) {
            continue;
        }
        if (node == dest_node) {
            forward_list_destroy(q);
            return cost;
        }
        cur.cur_cost = cost + 1;
        cur.cur_node = node - 1;
        forward_list_add_last(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == node - 1);
        cur.cur_node = 2 * node;
        forward_list_add_last(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == 2 * node);
    }
    forward_list_destroy(q);
    return -1;
}

static int test_puzzle_backwards(int start_node, int dest_node)
{
    forward_list q = forward_list_init(sizeof(struct pair));
    struct pair cur;
    cur.cur_node = start_node;
    cur.cur_cost = 0;
    assert(forward_list_is_empty(q));
    forward_list_add_first(q, &cur);
    assert(forward_list_size(q) == 1);
    while (!forward_list_is_empty(q)) {
        int node;
        int cost;
        forward_list_get_last(&cur, q);
        forward_list_remove_last(q);
        node = cur.cur_node;
        cost = cur.cur_cost;
        if (node > 2 * dest_node || node < 1) {
            continue;
        }
        if (node == dest_node) {
            forward_list_destroy(q);
            return cost;
        }
        cur.cur_cost = cost + 1;
        cur.cur_node = node - 1;
        forward_list_add_first(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == node - 1);
        cur.cur_node = 2 * node;
        forward_list_add_first(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == 2 * node);
    }
    forward_list_destroy(q);
    return -1;
}

struct big_object {
    int n;
    double d;
    signed char c[8];
};

static void test_big_object(void)
{
    int i;
    forward_list me = forward_list_init(sizeof(struct big_object));
    assert(me);
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        b.n = INT_MIN + i;
        b.d = i + 0.5;
        for (j = 0; j < 8; j++) {
            b.c[j] = (signed char) (SCHAR_MIN + i + j);
        }
        assert(forward_list_add_first(me, &b) == 0);
        b.n = -1;
        b.d = -1;
        for (j = 0; j < 8; j++) {
            b.c[j] = -1;
        }
    }
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        assert(forward_list_get_last(&b, me) == 0);
        assert(forward_list_remove_last(me) == 0);
        assert(b.n == INT_MIN + i);
        assert(b.d == i + 0.5);
        for (j = 0; j < 8; j++) {
            assert(b.c[j] == SCHAR_MIN + i + j);
        }
    }
    assert(!forward_list_destroy(me));
}

static void test_add_all(void)
{
    const size_t max_size = -1;
    size_t i;
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    forward_list me = forward_list_init(sizeof(int));
    assert(forward_list_add_all(me, arr, 0) == BK_OK);
    assert(forward_list_size(me) == 0);
    assert(forward_list_add_all(me, arr, 10) == BK_OK);
    assert(forward_list_size(me) == 10);
    for (i = 0; i < 10; i++) {
        int get;
        assert(forward_list_get_at(&get, me, i) == BK_OK);
        assert(get == (int) i + 1);
    }
    assert(forward_list_add_all(me, arr, 10) == BK_OK);
    assert(forward_list_size(me) == 20);
    assert(forward_list_remove_last(me) == BK_OK);
    assert(forward_list_add_all(me, arr, 10) == BK_OK);
    assert(forward_list_size(me) == 29);
    for (i = 0; i < 29; i++) {
        int get;
        int n = i;
        if (i >= 19) {
            n++;
        }
        assert(forward_list_get_at(&get, me, i) == BK_OK);
        assert(get == n % 10 + 1);
    }
    assert(forward_list_add_all(me, arr, max_size) == -BK_ERANGE);
    assert(forward_list_size(me) == 29);
#if STUB_MALLOC
    fail_malloc = 1;
    assert(forward_list_add_all(me, arr, 10) == -BK_ENOMEM);
    assert(forward_list_size(me) == 29);
    fail_malloc = 1;
    delay_fail_malloc = 5;
    assert(forward_list_add_all(me, arr, 10) == -BK_ENOMEM);
    assert(forward_list_size(me) == 29);
#endif
    forward_list_destroy(me);
}

void test_forward_list(void)
{
    test_invalid_init();
    test_basic();
    test_add_back();
#if STUB_MALLOC
    test_init_out_of_memory();
    test_add_first_out_of_memory();
    test_add_at_out_of_memory();
    test_add_last_out_of_memory();
#endif
    assert(test_puzzle_forwards(2, 5) == 4);
    assert(test_puzzle_forwards(2, 10) == 5);
    assert(test_puzzle_backwards(2, 5) == 4);
    assert(test_puzzle_backwards(2, 10) == 5);
    test_big_object();
    test_add_all();
    forward_list_destroy(NULL);
}
