#include "test.h"
#include "../src/include/unordered_set.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

static int hash_count;

static unsigned long hash_int(const void *const key)
{
    unsigned long hash = 17;
    hash_count++;
    hash = 31 * hash + *(int *) key;
    return hash;
}

static unsigned long bad_hash_int()
{
    return 5;
}

static void test_invalid_init(void)
{
    assert(!unordered_set_init(0, hash_int, compare_int));
    assert(!unordered_set_init(sizeof(int), NULL, compare_int));
    assert(!unordered_set_init(sizeof(int), hash_int, NULL));
}

static void test_put(unordered_set me)
{
    int val_arr[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    int key;
    int i;
    int j;
    assert(unordered_set_size(me) == 0);
    assert(unordered_set_is_empty(me));
    key = 4;
    unordered_set_put(me, &key);
    assert(unordered_set_size(me) == 1);
    unordered_set_put(me, &key);
    assert(unordered_set_size(me) == 1);
    assert(!unordered_set_is_empty(me));
    assert(unordered_set_contains(me, &key));
    key = 7;
    assert(!unordered_set_contains(me, &key));
    unordered_set_put(me, &key);
    assert(unordered_set_size(me) == 2);
    assert(unordered_set_contains(me, &key));
    for (i = 0; i < 10; i++) {
        unordered_set_put(me, &val_arr[i]);
        assert(unordered_set_contains(me, &val_arr[i]));
    }
    assert(unordered_set_size(me) == 9);
    for (i = 0; i < 10; i++) {
        assert(unordered_set_contains(me, &val_arr[i]));
    }
    for (i = -100; i < 100; i++) {
        int contains = 0;
        for (j = 0; j < 10; j++) {
            if (val_arr[j] == i) {
                contains = 1;
            }
        }
        assert(unordered_set_contains(me, &i) == contains);
    }
}

static void test_remove(unordered_set me)
{
    int num = -3;
    assert(!unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 9);
    assert(!unordered_set_contains(me, &num));
    num = 6;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 8);
    assert(!unordered_set_contains(me, &num));
    num = 4;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 7);
    assert(!unordered_set_contains(me, &num));
    num = 7;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 6);
    assert(!unordered_set_contains(me, &num));
    num = 9;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 5);
    assert(!unordered_set_contains(me, &num));
    num = -5;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 4);
    assert(!unordered_set_contains(me, &num));
    num = 0;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 3);
    assert(!unordered_set_contains(me, &num));
    num = 1;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 2);
    assert(!unordered_set_contains(me, &num));
    num = 5;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 1);
    assert(!unordered_set_contains(me, &num));
    num = 2;
    assert(unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 0);
    assert(!unordered_set_contains(me, &num));
}

static void test_stress_remove(unordered_set me)
{
    int i;
    for (i = 5000; i < 6000; i++) {
        unordered_set_put(me, &i);
        assert(unordered_set_contains(me, &i));
    }
    assert(unordered_set_size(me) == 1000);
    for (i = 5000; i < 6000; i++) {
        unordered_set_remove(me, &i);
        assert(!unordered_set_contains(me, &i));
    }
    assert(unordered_set_size(me) == 0);
    assert(unordered_set_is_empty(me));
    unordered_set_clear(me);
    assert(unordered_set_size(me) == 0);
    assert(unordered_set_is_empty(me));
}

static void test_stress_clear(unordered_set me)
{
    int i;
    int p = 0xfacade;
    for (i = 5000; i < 6000; i++) {
        unordered_set_put(me, &i);
        assert(unordered_set_contains(me, &i));
    }
    assert(unordered_set_size(me) == 1000);
    hash_count = 0;
    unordered_set_rehash(me);
    assert(hash_count == 1000);
    unordered_set_clear(me);
    assert(!unordered_set_remove(me, &p));
    assert(unordered_set_size(me) == 0);
    assert(unordered_set_is_empty(me));
}

static void test_basic(void)
{
    unordered_set me = unordered_set_init(sizeof(int), hash_int, compare_int);
    assert(me);
    test_put(me);
    test_remove(me);
    test_stress_remove(me);
    test_stress_clear(me);
    assert(!unordered_set_destroy(me));
}

static void test_bad_hash(void)
{
    int num;
    unordered_set me = unordered_set_init(sizeof(int), bad_hash_int,
                                          compare_int);
    assert(me);
    num = 1;
    unordered_set_put(me, &num);
    num = 2;
    unordered_set_put(me, &num);
    num = 3;
    unordered_set_put(me, &num);
    num = 4;
    unordered_set_put(me, &num);
    assert(unordered_set_size(me) == 4);
    unordered_set_put(me, &num);
    assert(unordered_set_size(me) == 4);
    assert(unordered_set_remove(me, &num));
    assert(!unordered_set_remove(me, &num));
    assert(unordered_set_size(me) == 3);
    unordered_set_rehash(me);
    assert(unordered_set_size(me) == 3);
    assert(!unordered_set_destroy(me));
}

static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!unordered_set_init(sizeof(int), hash_int, compare_int));
    fail_calloc = 1;
    assert(!unordered_set_init(sizeof(int), hash_int, compare_int));
}

static void test_rehash_out_of_memory(void)
{
    int key = 5;
    unordered_set me = unordered_set_init(sizeof(int), hash_int, compare_int);
    assert(me);
    unordered_set_put(me, &key);
    assert(unordered_set_size(me) == 1);
    assert(unordered_set_contains(me, &key));
    fail_calloc = 1;
    assert(unordered_set_rehash(me) == -ENOMEM);
    assert(unordered_set_size(me) == 1);
    assert(unordered_set_contains(me, &key));
    assert(!unordered_set_destroy(me));
}

static void test_put_out_of_memory(void)
{
    int key = 5;
    unordered_set me = unordered_set_init(sizeof(int), bad_hash_int,
                                          compare_int);
    assert(me);
    fail_malloc = 1;
    assert(unordered_set_put(me, &key) == -ENOMEM);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(unordered_set_put(me, &key) == -ENOMEM);
    assert(unordered_set_put(me, &key) == 0);
    key = 7;
    fail_malloc = 1;
    assert(unordered_set_put(me, &key) == -ENOMEM);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(unordered_set_put(me, &key) == -ENOMEM);
    assert(!unordered_set_destroy(me));
}

static void test_resize_out_of_memory(void)
{
    int i;
    unordered_set me = unordered_set_init(sizeof(int), hash_int, compare_int);
    for (i = 0; i < 5; i++) {
        assert(unordered_set_put(me, &i) == 0);
    }
    assert(unordered_set_size(me) == 5);
    i++;
    fail_calloc = 1;
    assert(unordered_set_put(me, &i) == -ENOMEM);
    assert(unordered_set_size(me) == 5);
    for (i = 0; i < 5; i++) {
        assert(unordered_set_contains(me, &i));
    }
    assert(!unordered_set_destroy(me));
}

static void test_clear_out_of_memory(void)
{
    int key = 5;
    unordered_set me = unordered_set_init(sizeof(int), hash_int, compare_int);
    assert(me);
    assert(unordered_set_put(me, &key) == 0);
    assert(unordered_set_size(me) == 1);
    assert(unordered_set_contains(me, &key));
    fail_calloc = 1;
    assert(unordered_set_clear(me) == -ENOMEM);
    assert(unordered_set_size(me) == 1);
    assert(unordered_set_contains(me, &key));
    assert(!unordered_set_destroy(me));
}

void test_unordered_set(void)
{
    test_invalid_init();
    test_basic();
    test_bad_hash();
#ifdef STUB_MALLOC
    test_init_out_of_memory();
    test_rehash_out_of_memory();
    test_put_out_of_memory();
    test_resize_out_of_memory();
    test_clear_out_of_memory();
#endif
}
