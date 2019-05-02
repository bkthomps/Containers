#include "test.h"
#include "../src/unordered_multimap.h"

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

static unsigned long bad_hash_int(const void *const key)
{
    return 5;
}

void test_unordered_multimap(void)
{
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    unordered_multimap me;
    int key;
    int value;
    int num;
    int count;
    int val;
    int i;
    int j;
    assert(!unordered_multimap_init(0, sizeof(int), hash_int, compare_int,
                                    compare_int));
    assert(!unordered_multimap_init(sizeof(int), 0, hash_int, compare_int,
                                    compare_int));
    assert(!unordered_multimap_init(sizeof(int), sizeof(int), NULL, compare_int,
                                    compare_int));
    assert(!unordered_multimap_init(sizeof(int), sizeof(int), hash_int, NULL,
                                    compare_int));
    assert(!unordered_multimap_init(sizeof(int), sizeof(int), hash_int,
                                    compare_int, NULL));
    me = unordered_multimap_init(sizeof(int), sizeof(int), hash_int,
                                 compare_int, compare_int);
    assert(me);
    assert(unordered_multimap_size(me) == 0);
    assert(unordered_multimap_is_empty(me));
    key = 4;
    value = 123;
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 1);
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 2);
    assert(!unordered_multimap_is_empty(me));
    assert(unordered_multimap_contains(me, &key));
    key = 7;
    assert(!unordered_multimap_contains(me, &key));
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 3);
    assert(unordered_multimap_contains(me, &key));
    unordered_multimap_remove(me, &key, &value);
    assert(unordered_multimap_size(me) == 2);
    assert(!unordered_multimap_contains(me, &key));
    key = 4;
    unordered_multimap_remove(me, &key, &value);
    assert(unordered_multimap_size(me) == 1);
    unordered_multimap_remove(me, &key, &value);
    assert(unordered_multimap_size(me) == 0);
    for (i = 0; i < 10; i++) {
        unordered_multimap_put(me, &c[i], &value);
        assert(unordered_multimap_contains(me, &c[i]));
    }
    assert(unordered_multimap_size(me) == 10);
    for (i = 0; i < 10; i++) {
        assert(unordered_multimap_contains(me, &c[i]));
    }
    for (i = -100; i < 100; i++) {
        bool contains = false;
        for (j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(unordered_multimap_contains(me, &i) == contains);
    }
    num = -3;
    assert(!unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 10);
    assert(!unordered_multimap_contains(me, &num));
    num = 6;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 9);
    assert(!unordered_multimap_contains(me, &num));
    num = 4;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 8);
    assert(!unordered_multimap_contains(me, &num));
    num = 7;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 7);
    assert(!unordered_multimap_contains(me, &num));
    num = 9;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 6);
    assert(!unordered_multimap_contains(me, &num));
    num = -5;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 5);
    assert(!unordered_multimap_contains(me, &num));
    num = 0;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 4);
    assert(!unordered_multimap_contains(me, &num));
    num = 1;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 3);
    assert(!unordered_multimap_contains(me, &num));
    num = 5;
    assert(unordered_multimap_count(me, &num) == 2);
    unordered_multimap_get_start(me, &num);
    count = 0;
    val = 0xdeadbeef;
    while (unordered_multimap_get_next(&val, me)) {
        count++;
        assert(val == 123);
        val = 0xdeadbeef;
    }
    assert(count == 2);
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 2);
    assert(unordered_multimap_contains(me, &num));
    num = 2;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 1);
    assert(!unordered_multimap_contains(me, &num));
    num = 5;
    assert(unordered_multimap_remove(me, &num, &value));
    assert(unordered_multimap_size(me) == 0);
    assert(!unordered_multimap_contains(me, &num));
    /* Add a lot of items and remove individually. */
    for (i = 5000; i < 6000; i++) {
        unordered_multimap_put(me, &i, &value);
        assert(unordered_multimap_contains(me, &i));
    }
    assert(unordered_multimap_size(me) == 1000);
    for (i = 5000; i < 6000; i++) {
        unordered_multimap_remove(me, &i, &value);
        assert(!unordered_multimap_contains(me, &i));
    }
    assert(unordered_multimap_size(me) == 0);
    assert(unordered_multimap_is_empty(me));
    unordered_multimap_clear(me);
    assert(unordered_multimap_size(me) == 0);
    assert(unordered_multimap_is_empty(me));
    /* Add a lot of items and clear. */
    for (i = 5000; i < 6000; i++) {
        unordered_multimap_put(me, &i, &value);
        assert(unordered_multimap_contains(me, &i));
    }
    assert(unordered_multimap_size(me) == 1000);
    hash_count = 0;
    unordered_multimap_rehash(me);
    assert(hash_count == 1000);
    unordered_multimap_clear(me);
    key = 0xdeadbeef;
    assert(!unordered_multimap_remove(me, &key, &value));
    assert(unordered_multimap_size(me) == 0);
    assert(unordered_multimap_is_empty(me));
    key = 5;
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 1);
    key = 7;
    for (i = 0; i < 10; i++) {
        unordered_multimap_put(me, &key, &value);
    }
    assert(unordered_multimap_size(me) == 11);
    unordered_multimap_remove_all(me, &key);
    assert(unordered_multimap_size(me) == 1);
    me = unordered_multimap_destroy(me);
    assert(!me);
    me = unordered_multimap_init(sizeof(int), sizeof(int), bad_hash_int,
                                 compare_int, compare_int);
    key = 1;
    unordered_multimap_put(me, &key, &value);
    key = 2;
    unordered_multimap_put(me, &key, &value);
    key = 3;
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 3);
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 4);
    key = 4;
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 5);
    assert(unordered_multimap_remove(me, &key, &value));
    assert(!unordered_multimap_remove(me, &key, &value));
    assert(unordered_multimap_size(me) == 4);
    unordered_multimap_rehash(me);
    assert(unordered_multimap_size(me) == 4);
    me = unordered_multimap_init(sizeof(int), sizeof(int), bad_hash_int,
                                 compare_int, compare_int);
    key = 1;
    unordered_multimap_put(me, &key, &value);
    key = 2;
    unordered_multimap_put(me, &key, &value);
    key = 3;
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 3);
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 4);
    key = 4;
    unordered_multimap_put(me, &key, &value);
    assert(unordered_multimap_size(me) == 5);
    assert(unordered_multimap_remove_all(me, &key));
    assert(!unordered_multimap_remove_all(me, &key));
    assert(unordered_multimap_size(me) == 4);
    unordered_multimap_rehash(me);
    assert(unordered_multimap_size(me) == 4);
    unordered_multimap_clear(me);
    assert(unordered_multimap_size(me) == 0);
    assert(!unordered_multimap_remove_all(me, &key));
    assert(unordered_multimap_size(me) == 0);
    me = unordered_multimap_init(sizeof(int), sizeof(int), bad_hash_int,
                                 compare_int, compare_int);
    key = 1;
    value = 10;
    unordered_multimap_put(me, &key, &value);
    key = 2;
    value = 11;
    unordered_multimap_put(me, &key, &value);
    key = 3;
    value = 12;
    unordered_multimap_put(me, &key, &value);
    key = 4;
    value = 14;
    unordered_multimap_put(me, &key, &value);
    key = 3;
    value = 13;
    unordered_multimap_put(me, &key, &value);
    unordered_multimap_get_start(me, &key);
    value = 20;
    unordered_multimap_get_next(&value, me);
    assert(value == 12);
    unordered_multimap_get_next(&value, me);
    assert(value == 13);
    value = 30;
    assert(!unordered_multimap_get_next(&value, me));
    assert(value == 30);
}
