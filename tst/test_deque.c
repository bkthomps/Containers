#include "test.h"
#include "../src/include/deque.h"

static void test_invalid_init(void)
{
    const size_t max_size = -1;
    assert(!deque_init(0));
    assert(!deque_init(max_size));
}

static void test_copy(deque me)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get_arr[10] = {0};
    int trimmed[5] = {0};
    int get;
    int i;
    for (i = 0; i < 10; i++) {
        deque_push_front(me, &val[i]);
        get = 0xfacade;
        deque_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(deque_size(me) == 10);
    assert(!deque_is_empty(me));
    deque_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        get = 0xfacade;
        deque_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    get = 0xfacade;
    deque_trim(me);
    for (i = 0; i < 7; i++) {
        deque_pop_back(&get, me);
        assert(get == val[i]);
    }
    deque_copy_to_array(trimmed, me);
    assert(deque_size(me) == 3);
    for (i = 0; i < 3; i++) {
        get = 0xfacade;
        deque_get_at(&get, me, i);
        assert(get == 10 - i);
        assert(10 - i == trimmed[i]);
    }
}

static void test_linear_operations(deque me)
{
    int arr[3] = {0};
    int get;
    int set;
    int add = 3;
    deque_push_back(me, &add);
    add = -2;
    deque_push_back(me, &add);
    add = -1;
    deque_push_back(me, &add);
    assert(deque_size(me) == 6);
    get = 0xfacade;
    deque_get_first(&get, me);
    assert(get == 10);
    get = 0xfacade;
    deque_get_at(&get, me, 0);
    assert(get == 10);
    deque_get_at(&get, me, 1);
    assert(get == 9);
    deque_get_at(&get, me, 2);
    assert(get == 8);
    deque_get_at(&get, me, 3);
    assert(get == 3);
    deque_get_at(&get, me, 4);
    assert(get == -2);
    deque_get_at(&get, me, 5);
    assert(get == -1);
    get = 0xfacade;
    deque_get_last(&get, me);
    assert(get == -1);
    deque_pop_front(&get, me);
    assert(get == 10);
    deque_pop_back(&get, me);
    assert(get == -1);
    deque_pop_back(&get, me);
    assert(get == -2);
    assert(deque_size(me) == 3);
    get = 345;
    deque_get_first(&get, me);
    assert(get == 9);
    deque_get_at(&get, me, 1);
    assert(get == 8);
    deque_get_last(&get, me);
    assert(get == 3);
    set = 12;
    deque_set_first(me, &set);
    set = 13;
    deque_set_at(me, 1, &set);
    set = 14;
    deque_set_last(me, &set);
    deque_copy_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    deque_set_at(me, 0, &set);
    set = -6;
    deque_set_at(me, 1, &set);
    set = -7;
    deque_set_at(me, 2, &set);
    deque_copy_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
}

static void test_invalid_input(deque me)
{
    int set;
    assert(deque_set_at(me, 4, &set) == -EINVAL);
    assert(deque_get_at(&set, me, 4) == -EINVAL);
    assert(deque_set_at(me, -1, &set) == -EINVAL);
    assert(deque_get_at(&set, me, -1) == -EINVAL);
}

static void test_basic(void)
{
    deque me = deque_init(sizeof(int));
    assert(me);
    assert(deque_size(me) == 0);
    assert(deque_is_empty(me));
    test_copy(me);
    test_linear_operations(me);
    test_invalid_input(me);
    deque_clear(me);
    assert(deque_size(me) == 0);
    assert(deque_is_empty(me));
    assert(!deque_destroy(me));
}

static void test_trim(void)
{
    deque me = deque_init(sizeof(int));
    int i;
    assert(deque_is_empty(me));
    deque_trim(me);
    assert(deque_is_empty(me));
    for (i = 0; i < 100; i++) {
        int get;
        deque_push_back(me, &i);
        deque_pop_front(&get, me);
    }
    assert(deque_is_empty(me));
    deque_trim(me);
    assert(deque_is_empty(me));
    for (i = 0; i < 100; i++) {
        deque_push_back(me, &i);
    }
    for (i = 0; i < 100; i++) {
        int get = 0xfacade;
        deque_get_at(&get, me, i);
    }
    assert(deque_size(me) == 100);
    deque_trim(me);
    assert(deque_size(me) == 100);
    deque_destroy(me);
}

static void test_stress(void)
{
    int i;
    deque me = deque_init(sizeof(int));
    for (i = -1000; i < 1000; i++) {
        int val = 7 * i;
        deque_push_back(me, &val);
    }
    assert(deque_size(me) == 2000);
    for (i = -1000; i < 1000; i++) {
        int get = 0xfacade;
        deque_get_at(&get, me, i + 1000);
        assert(get == 7 * i);
    }
    assert(deque_size(me) == 2000);
    deque_clear(me);
    assert(deque_size(me) == 0);
    assert(deque_is_empty(me));
    for (i = -1000; i < 1000; i++) {
        int val = 7 * i;
        deque_push_front(me, &val);
    }
    assert(deque_size(me) == 2000);
    for (i = -1000; i < 1000; i++) {
        int get = 0xfacade;
        deque_get_at(&get, me, i + 1000);
        assert(get == -7 * (i + 1));
    }
    assert(deque_size(me) == 2000);
    deque_destroy(me);
}

static void test_array_copy(void)
{
    size_t i = 0xfacade;
    size_t arr_sz = 9000;
    size_t arr[9000] = {0};
    deque me = deque_init(sizeof *arr);
    assert(sizeof arr / sizeof *arr == arr_sz);
    deque_copy_to_array(&i, me);
    assert(i == 0xfacade);
    for (i = 0; i < arr_sz; i++) {
        deque_push_back(me, &i);
    }
    for (i = 0; i < arr_sz; i++) {
        size_t get = 0xfacade;
        deque_get_at(&get, me, i);
        assert(get == i);
    }
    deque_copy_to_array(&arr, me);
    for (i = 0; i < arr_sz; i++) {
        assert(arr[i] == i);
    }
    deque_clear(me);
    i = 0xfacade;
    deque_copy_to_array(&i, me);
    assert(i == 0xfacade);
    for (i = 0; i < arr_sz; i++) {
        deque_push_front(me, &i);
    }
    deque_copy_to_array(&arr, me);
    for (i = 0; i < arr_sz; i++) {
        assert(arr[i] == arr_sz - i - 1);
    }
    for (i = 0; i < arr_sz / 2; i++) {
        size_t get = 0xfacade;
        deque_pop_front(&get, me);
        assert(get == arr_sz - i - 1);
    }
    for (i = 0; i < arr_sz / 2; i++) {
        size_t get = 0xfacade;
        deque_push_front(me, &i);
        deque_get_first(&get, me);
        assert(get == i);
    }
    deque_destroy(me);
}

struct large_data {
    double data[100];
};

static void test_large_elements(void)
{
    int i;
    struct large_data data = {0};
    struct large_data get = {0};
    deque me = deque_init(sizeof(struct large_data));
    for (i = 0; i < 100; i++) {
        data.data[i] = i + 0.5;
    }
    deque_push_back(me, &data);
    deque_pop_back(&get, me);
    for (i = 0; i < 100; i++) {
        assert(data.data[i] > i + 0.4);
        assert(data.data[i] < i + 0.6);
    }
    deque_destroy(me);
}

#if STUB_MALLOC
static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!deque_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(!deque_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 2;
    assert(!deque_init(sizeof(int)));
}
#endif

#if STUB_MALLOC
static void test_trim_out_of_memory(void)
{
    deque me = deque_init(sizeof(int));
    int i;
    for (i = 0; i < 32; i++) {
        deque_push_back(me, &i);
    }
    assert(deque_size(me) == 32);
    fail_malloc = 1;
    assert(deque_trim(me) == -ENOMEM);
    for (i = 0; i < 32; i++) {
        int get = 0xfacade;
        deque_get_at(&get, me, i);
        assert(get == i);
    }
    assert(deque_size(me) == 32);
    assert(!deque_destroy(me));
}
#endif

#if STUB_MALLOC
static void test_push_front_out_of_memory(void)
{
    deque me = deque_init(sizeof(int));
    int i;
    for (i = 0; i < 4096; i++) {
        deque_push_front(me, &i);
    }
    fail_realloc = 1;
    assert(deque_push_front(me, &i) == -ENOMEM);
    delay_fail_realloc = 1;
    fail_malloc = 1;
    assert(deque_push_front(me, &i) == -ENOMEM);
    assert(!deque_destroy(me));
}
#endif

#if STUB_MALLOC
static void test_push_back_out_of_memory(void)
{
    deque me = deque_init(sizeof(int));
    int i;
    for (i = 0; i < 4096; i++) {
        deque_push_back(me, &i);
    }
    fail_realloc = 1;
    assert(deque_push_back(me, &i) == -ENOMEM);
    delay_fail_realloc = 1;
    fail_malloc = 1;
    assert(deque_push_back(me, &i) == -ENOMEM);
    assert(!deque_destroy(me));
}
#endif

#if STUB_MALLOC
static void test_clear_out_of_memory(void)
{
    deque me = deque_init(sizeof(int));
    int i;
    for (i = 0; i < 32; i++) {
        deque_push_back(me, &i);
    }
    assert(deque_size(me) == 32);
    fail_malloc = 1;
    assert(deque_clear(me) == -ENOMEM);
    for (i = 0; i < 32; i++) {
        int get = 0xfacade;
        deque_get_at(&get, me, i);
        assert(get == i);
    }
    assert(deque_size(me) == 32);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(deque_clear(me) == -ENOMEM);
    for (i = 0; i < 32; i++) {
        int get = 0xfacade;
        deque_get_at(&get, me, i);
        assert(get == i);
    }
    assert(deque_size(me) == 32);
    assert(!deque_destroy(me));
}
#endif

void test_single_full_block(void)
{
    int i;
    int num = 5;
    deque me = deque_init(sizeof(int));
    for (i = 0; i < 5; i++) {
        deque_push_front(me, &num);
    }
    for (i = 0; i < 3; i++) {
        deque_push_back(me, &num);
    }
    for (i = 0; i < 8; i++) {
        deque_pop_back(&num, me);
    }
    deque_trim(me);
    assert(deque_size(me) == 0);
    deque_destroy(me);
}

struct pair {
    int cur_node;
    int cur_cost;
};

static int test_puzzle_forwards(int start_node, int dest_node)
{
    deque q = deque_init(sizeof(struct pair));
    struct pair cur;
    cur.cur_node = start_node;
    cur.cur_cost = 0;
    assert(deque_is_empty(q));
    deque_push_back(q, &cur);
    assert(deque_size(q) == 1);
    while (!deque_is_empty(q)) {
        int node;
        int cost;
        deque_pop_front(&cur, q);
        node = cur.cur_node;
        cost = cur.cur_cost;
        if (node > 2 * dest_node || node < 1) {
            continue;
        }
        if (node == dest_node) {
            deque_destroy(q);
            return cost;
        }
        cur.cur_cost = cost + 1;
        cur.cur_node = node - 1;
        deque_push_back(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == node - 1);
        cur.cur_node = 2 * node;
        deque_push_back(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == 2 * node);
    }
    deque_destroy(q);
    return -1;
}

static int test_puzzle_backwards(int start_node, int dest_node)
{
    deque q = deque_init(sizeof(struct pair));
    struct pair cur;
    cur.cur_node = start_node;
    cur.cur_cost = 0;
    assert(deque_is_empty(q));
    deque_push_back(q, &cur);
    assert(deque_size(q) == 1);
    while (!deque_is_empty(q)) {
        int node;
        int cost;
        deque_pop_back(&cur, q);
        node = cur.cur_node;
        cost = cur.cur_cost;
        if (node > 2 * dest_node || node < 1) {
            continue;
        }
        if (node == dest_node) {
            deque_destroy(q);
            return cost;
        }
        cur.cur_cost = cost + 1;
        cur.cur_node = node - 1;
        deque_push_front(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == node - 1);
        cur.cur_node = 2 * node;
        deque_push_front(q, &cur);
        assert(cur.cur_cost == cost + 1);
        assert(cur.cur_node == 2 * node);
    }
    deque_destroy(q);
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
    deque me = deque_init(sizeof(struct big_object));
    assert(me);
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        b.n = INT_MIN + i;
        b.d = i + 0.5;
        for (j = 0; j < 8; j++) {
            b.c[j] = (signed char) (SCHAR_MIN + i + j);
        }
        assert(deque_push_front(me, &b) == 0);
        b.n = -1;
        b.d = -1;
        for (j = 0; j < 8; j++) {
            b.c[j] = -1;
        }
    }
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        assert(deque_pop_back(&b, me) == 0);
        assert(b.n == INT_MIN + i);
        assert(b.d == i + 0.5);
        for (j = 0; j < 8; j++) {
            assert(b.c[j] == SCHAR_MIN + i + j);
        }
    }
    assert(!deque_destroy(me));
}

static void test_add_all(int big_arr_size)
{
    int i;
    double small_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double *big_array = malloc(big_arr_size * sizeof(double));
    deque me = deque_init(sizeof(double));
    for (i = 0; i < big_arr_size; i++) {
        big_array[i] = i + 10;
    }
    assert(deque_add_all(me, small_array, 10) == BK_OK);
    assert(deque_size(me) == 10);
    for (i = 0; i < 9; i++) {
        double get;
        assert(deque_get_at(&get, me, i) == BK_OK);
        assert(small_array[i] == i + 1);
        assert(get == small_array[i]);
    }
    assert(deque_add_all(me, big_array, big_arr_size) == BK_OK);
    for (i = 10; i < big_arr_size; i++) {
        double get;
        assert(deque_get_at(&get, me, i) == BK_OK);
        assert(big_array[i - 10] == i);
        assert(get == big_array[i - 10]);
    }
    deque_destroy(me);
    free(big_array);
}

static void test_add_all_failure(void)
{
    const size_t big_arr_size = 2000;
    size_t i;
    int small_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double *big_array = malloc(big_arr_size * sizeof(double));
    deque me = deque_init(sizeof(int));
    for (i = 0; i < big_arr_size; i++) {
        big_array[i] = (int) i + 10;
    }
    assert(deque_add_all(me, small_array, 0) == BK_OK);
    assert(deque_size(me) == 0);
    assert(deque_add_all(me, small_array, -1) == -BK_ERANGE);
    assert(deque_size(me) == 0);
    for (i = 0; i < 200; i++) {
        assert(deque_add_all(me, small_array, 10) == BK_OK);
    }
    assert(deque_size(me) == 2000);
    for (i = 0; i < 2000; i++) {
        int get;
        assert(deque_pop_back(&get, me) == BK_OK);
        assert(get == 10 - (int) i % 10);
    }
    assert(deque_size(me) == 0);
    for (i = 0; i < 200; i++) {
        assert(deque_add_all(me, small_array, 10) == BK_OK);
    }
    assert(deque_size(me) == 2000);
#if STUB_MALLOC
    fail_realloc = 1;
    assert(deque_add_all(me, small_array, 4000) == -BK_ENOMEM);
    assert(deque_size(me) == 2000);
    fail_malloc = 1;
    assert(deque_add_all(me, big_array, 2000) == -BK_ENOMEM);
    assert(deque_size(me) == 2000);
#endif
    deque_destroy(me);
    free(big_array);
}

static void test_block_reuse_forwards(void)
{
    size_t i;
    size_t queue_size = 1500;
    deque queue = deque_init(sizeof(double));
    for (i = 0; i < queue_size; i++) {
        double d = i;
        assert(deque_push_back(queue, &d) == BK_OK);
    }
    for (i = 0; i < queue_size; i++) {
        double d = i;
        double get;
        assert(deque_push_back(queue, &d) == BK_OK);
        assert(deque_pop_front(&get, queue) == BK_OK);
        assert(get == d);
    }
    deque_destroy(queue);
}

static void test_block_reuse_backwards(void)
{
    size_t i;
    size_t queue_size = 1500;
    deque queue = deque_init(sizeof(double));
    for (i = 0; i < queue_size; i++) {
        double d = i;
        assert(deque_push_front(queue, &d) == BK_OK);
    }
    for (i = 0; i < queue_size; i++) {
        double d = i;
        double get;
        assert(deque_push_front(queue, &d) == BK_OK);
        assert(deque_pop_back(&get, queue) == BK_OK);
        assert(get == d);
    }
    deque_destroy(queue);
}

static void test_trim_both_sides(void)
{
    int i;
    deque me = deque_init(sizeof(int));
    for (i = 999; i >= 0; i--) {
        assert(deque_push_front(me, &i) == BK_OK);
        assert(deque_push_back(me, &i) == BK_OK);
    }
    assert(deque_size(me) == 2000);
    assert(deque_trim(me) == BK_OK);
    assert(deque_size(me) == 2000);
    for (i = 0; i < 500; i++) {
        int get = 0xfacade;
        assert(deque_pop_front(&get, me) == BK_OK);
        assert(get == i);
        get = 0xfacade;
        assert(deque_pop_back(&get, me) == BK_OK);
        assert(get == i);
    }
    assert(deque_size(me) == 1000);
    assert(deque_trim(me) == BK_OK);
    assert(deque_size(me) == 1000);
    for (i = 500; i < 1000; i++) {
        int get = 0xfacade;
        assert(deque_pop_front(&get, me) == BK_OK);
        assert(get == i);
        get = 0xfacade;
        assert(deque_pop_back(&get, me) == BK_OK);
        assert(get == i);
    }
    assert(deque_size(me) == 0);
    assert(deque_trim(me) == BK_OK);
    assert(deque_size(me) == 0);
    deque_destroy(me);
}

void test_deque(void)
{
    int i;
    test_invalid_init();
    test_basic();
    test_trim();
    test_stress();
    test_array_copy();
    test_large_elements();
#if STUB_MALLOC
    /* These OOM tests rely on this being true. */
    assert(sizeof(int) == 4);
    test_init_out_of_memory();
    test_trim_out_of_memory();
    test_push_front_out_of_memory();
    test_push_back_out_of_memory();
    test_clear_out_of_memory();
#endif
    test_single_full_block();
    assert(test_puzzle_forwards(2, 5) == 4);
    assert(test_puzzle_forwards(2, 10) == 5);
    assert(test_puzzle_forwards(100, 1000) == 42);
    assert(test_puzzle_backwards(2, 5) == 4);
    assert(test_puzzle_backwards(2, 10) == 5);
    assert(test_puzzle_backwards(100, 1000) == 42);
    test_big_object();
    for (i = 1; i < 6000; i++) {
        test_add_all(i);
    }
    test_add_all_failure();
    test_block_reuse_forwards();
    test_block_reuse_backwards();
    test_trim_both_sides();
    deque_destroy(NULL);
}
