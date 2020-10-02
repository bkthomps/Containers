#include "test.h"
#include "../src/include/vector.h"

static void test_invalid_init(void)
{
    const size_t compare_int = -1;
    assert(!vector_init(0));
    assert(!vector_init(compare_int));
}

static void test_adding(vector me)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get_arr[10] = {0};
    int *data;
    int get;
    int i;
    for (i = 0; i < 10; i++) {
        vector_add_first(me, &val[i]);
        get = 0;
        vector_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(vector_size(me) == 10);
    assert(!vector_is_empty(me));
    vector_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        get = 0;
        vector_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    data = vector_get_data(me);
    for (i = 0; i < 10; i++) {
        assert(data[i] == val[9 - i]);
    }
    assert(vector_capacity(me) >= vector_size(me));
}

static void test_trim(vector me)
{
    int trimmed[5] = {0};
    int i;
    vector_trim(me);
    vector_reserve(me, 3);
    for (i = 0; i < 7; i++) {
        vector_remove_last(me);
    }
    vector_copy_to_array(trimmed, me);
    assert(vector_size(me) == 3);
    for (i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
}

static void test_linear_operations(vector me)
{
    int arr[3] = {0};
    int get;
    int set;
    int add = 3;
    assert(vector_size(me) == 3);
    vector_add_last(me, &add);
    add = -1;
    vector_add_at(me, 1, &add);
    add = -2;
    vector_add_last(me, &add);
    assert(vector_size(me) == 6);
    get = 0xfacade;
    vector_get_first(&get, me);
    assert(get == 10);
    get = 0xfacade;
    vector_get_at(&get, me, 0);
    assert(get == 10);
    vector_get_at(&get, me, 1);
    assert(get == -1);
    vector_get_at(&get, me, 2);
    assert(get == 9);
    vector_get_at(&get, me, 3);
    assert(get == 8);
    vector_get_at(&get, me, 4);
    assert(get == 3);
    vector_get_at(&get, me, 5);
    assert(get == -2);
    get = 0xfacade;
    vector_get_last(&get, me);
    assert(get == -2);
    vector_remove_first(me);
    vector_remove_at(me, 2);
    vector_remove_last(me);
    assert(vector_size(me) == 3);
    get = 345;
    vector_get_first(&get, me);
    assert(get == -1);
    vector_get_at(&get, me, 1);
    assert(get == 9);
    vector_get_last(&get, me);
    assert(get == 3);
    set = 12;
    vector_set_first(me, &set);
    set = 13;
    vector_set_at(me, 1, &set);
    set = 14;
    vector_set_last(me, &set);
    vector_copy_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    vector_set_at(me, 0, &set);
    set = -6;
    vector_set_at(me, 1, &set);
    set = -7;
    vector_set_at(me, 2, &set);
    vector_copy_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
}

static void test_invalid_operations(vector me)
{
    int set;
    int i;
    assert(vector_reserve(me, 100) == 0);
    assert(vector_set_at(me, 4, &set) == -EINVAL);
    assert(vector_get_at(&set, me, 4) == -EINVAL);
    assert(vector_remove_at(me, 4) == -EINVAL);
    assert(vector_add_at(me, 5, &set) == -EINVAL);
    assert(vector_set_at(me, -1, &set) == -EINVAL);
    assert(vector_get_at(&set, me, -1) == -EINVAL);
    assert(vector_remove_at(me, -1) == -EINVAL);
    assert(vector_add_at(me, -1, &set) == -EINVAL);
    vector_clear(me);
    for (i = 0; i < 32; i++) {
        vector_add_last(me, &i);
    }
    vector_clear(me);
    assert(vector_capacity(me) == 8);
    assert(vector_size(me) == 0);
    assert(vector_is_empty(me));
    assert(vector_remove_first(me) == -EINVAL);
    assert(vector_remove_last(me) == -EINVAL);
}

static void test_basic(void)
{
    vector me = vector_init(sizeof(int));
    assert(me);
    assert(vector_size(me) == 0);
    assert(vector_is_empty(me));
    test_adding(me);
    test_trim(me);
    test_linear_operations(me);
    test_invalid_operations(me);
    assert(!vector_destroy(me));
}

static void test_vector_of_vectors(void)
{
    /* Test using a vector of vectors of ints. */
    vector outer = vector_init(sizeof(vector));
    /* Add vectors to the outer vector. */
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        vector inner = vector_init(sizeof(int));
        for (j = 1; j <= 10; j++) {
            vector_add_last(inner, &j);
        }
        assert(vector_size(inner) == 10);
        vector_add_last(outer, &inner);
    }
    assert(vector_size(outer) == 5);
    /* Delete the vectors in the outer vector. */
    for (i = 0; i < 5; i++) {
        vector inner = NULL;
        vector_get_first(&inner, outer);
        for (j = 0; j < 10; j++) {
            int num = 0xfacade;
            vector_get_at(&num, inner, j);
            assert(num == j + 1);
        }
        vector_remove_first(outer);
        vector_destroy(inner);
    }
    assert(vector_is_empty(outer));
    vector_destroy(outer);
}

static void test_dynamic(void)
{
    char **str = malloc(5 * sizeof(char **));
    int i;
    int j;
    vector str_vector;
    for (i = 0; i < 5; i++) {
        str[i] = malloc(10 * sizeof(char *));
        for (j = 0; j < 9; j++) {
            str[i][j] = (char) ('a' + i);
        }
        str[i][9] = '\0';
    }
    str_vector = vector_init(sizeof(char *));
    assert(str_vector);
    for (i = 0; i < 5; i++) {
        vector_add_last(str_vector, &str[i]);
    }
    assert(vector_size(str_vector) == 5);
    for (i = 0; i < 5; i++) {
        char *retrieve = NULL;
        vector_get_first(&retrieve, str_vector);
        vector_remove_first(str_vector);
        assert(strcmp(retrieve, str[i]) == 0);
        free(retrieve);
    }
    free(str);
    assert(vector_size(str_vector) == 0);
    assert(!vector_destroy(str_vector));
}

static void test_reserve_erange(void)
{
    const size_t max_size = -1;
    vector me = vector_init(sizeof(int));
    assert(vector_reserve(me, max_size) == -ERANGE);
    vector_destroy(me);
}

#if STUB_MALLOC
static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!vector_init(sizeof(int)));
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(!vector_init(sizeof(int)));
}
#endif

#if STUB_MALLOC
static void test_set_space_out_of_memory(void)
{
    vector me = vector_init(sizeof(int));
    int i;
    for (i = 0; i < 7; i++) {
        assert(vector_add_last(me, &i) == 0);
    }
    fail_realloc = 1;
    assert(vector_reserve(me, 9) == -ENOMEM);
    assert(vector_size(me) == 7);
    assert(vector_capacity(me) == 8);
    for (i = 0; i < 7; i++) {
        int get = 0xfacade;
        vector_get_at(&get, me, i);
        assert(get == i);
    }
    assert(!vector_destroy(me));
}
#endif

#if STUB_MALLOC
static void test_add_out_of_memory(void)
{
    vector me = vector_init(sizeof(int));
    int i;
    for (i = 0; i < 8; i++) {
        assert(vector_add_last(me, &i) == 0);
    }
    i++;
    fail_realloc = 1;
    assert(vector_add_last(me, &i) == -ENOMEM);
    assert(vector_size(me) == 8);
    assert(vector_capacity(me) == 8);
    for (i = 0; i < 8; i++) {
        int get = 0xfacade;
        vector_get_at(&get, me, i);
        assert(get == i);
    }
    assert(!vector_destroy(me));
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
    vector me = vector_init(sizeof(struct big_object));
    assert(me);
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        b.n = INT_MIN + i;
        b.d = i + 0.5;
        for (j = 0; j < 8; j++) {
            b.c[j] = (signed char) (SCHAR_MIN + i + j);
        }
        assert(vector_add_first(me, &b) == 0);
        b.n = -1;
        b.d = -1;
        for (j = 0; j < 8; j++) {
            b.c[j] = -1;
        }
    }
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        assert(vector_get_last(&b, me) == 0);
        assert(vector_remove_last(me) == 0);
        assert(b.n == INT_MIN + i);
        assert(b.d == i + 0.5);
        for (j = 0; j < 8; j++) {
            assert(b.c[j] == SCHAR_MIN + i + j);
        }
    }
    assert(!vector_destroy(me));
}

static void test_add_all(void)
{
    size_t i;
    const size_t max_size = -1;
    double small_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector me = vector_init(sizeof(double));
    assert(vector_add_all(me, small_array, 10) == BK_OK);
    assert(vector_size(me) == 10);
    for (i = 0; i < 10; i++) {
        double get;
        assert(vector_get_at(&get, me, i) == BK_OK);
        assert(get == i + 1);
    }
    assert(vector_add_all(me, small_array, max_size) == -BK_ERANGE);
    assert(vector_add_all(me, small_array, max_size - 10) == -BK_ERANGE);
    assert(vector_add_all(me, small_array, max_size - 20) == -BK_ERANGE);
    vector_destroy(me);
}

void test_vector(void)
{
    test_invalid_init();
    test_basic();
    test_vector_of_vectors();
    test_dynamic();
    test_reserve_erange();
#if STUB_MALLOC
    test_init_out_of_memory();
    test_set_space_out_of_memory();
    test_add_out_of_memory();
#endif
    test_big_object();
    test_add_all();
    vector_destroy(NULL);
}
