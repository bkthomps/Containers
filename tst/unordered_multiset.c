#include "test.h"
#include "../src/unordered_multiset.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

static int hash_count;

static unsigned long hash_int(const void *const key)
{
    hash_count++;
    unsigned long hash = 17;
    hash = 31 * hash + *(int *) key;
    return hash;
}

void test_unordered_multiset(void)
{
    assert(!unordered_multiset_init(0, hash_int, compare_int));
    assert(!unordered_multiset_init(sizeof(int), NULL, compare_int));
    assert(!unordered_multiset_init(sizeof(int), hash_int, NULL));
    unordered_multiset me =
            unordered_multiset_init(sizeof(int), hash_int, compare_int);
    assert(me);
    assert(unordered_multiset_size(me) == 0);
    assert(unordered_multiset_is_empty(me));
    int key = 4;
    unordered_multiset_put(me, &key);
    assert(unordered_multiset_size(me) == 1);
    unordered_multiset_put(me, &key);
    assert(unordered_multiset_size(me) == 2);
    assert(!unordered_multiset_is_empty(me));
    assert(unordered_multiset_contains(me, &key));
    key = 7;
    assert(!unordered_multiset_contains(me, &key));
    unordered_multiset_put(me, &key);
    assert(unordered_multiset_size(me) == 3);
    assert(unordered_multiset_contains(me, &key));
    unordered_multiset_remove(me, &key);
    assert(unordered_multiset_size(me) == 2);
    assert(!unordered_multiset_contains(me, &key));
    key = 4;
    unordered_multiset_remove(me, &key);
    assert(unordered_multiset_size(me) == 1);
    unordered_multiset_remove(me, &key);
    assert(unordered_multiset_size(me) == 0);
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        unordered_multiset_put(me, &c[i]);
        assert(unordered_multiset_contains(me, &c[i]));
    }
    assert(unordered_multiset_size(me) == 10);
    for (int i = 0; i < 10; i++) {
        assert(unordered_multiset_contains(me, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(unordered_multiset_contains(me, &i) == contains);
    }
    int num = -3;
    assert(!unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 10);
    assert(!unordered_multiset_contains(me, &num));
    num = 6;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 9);
    assert(!unordered_multiset_contains(me, &num));
    num = 4;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 8);
    assert(!unordered_multiset_contains(me, &num));
    num = 7;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 7);
    assert(!unordered_multiset_contains(me, &num));
    num = 9;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 6);
    assert(!unordered_multiset_contains(me, &num));
    num = -5;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 5);
    assert(!unordered_multiset_contains(me, &num));
    num = 0;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 4);
    assert(!unordered_multiset_contains(me, &num));
    num = 1;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 3);
    assert(!unordered_multiset_contains(me, &num));
    num = 5;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 2);
    assert(unordered_multiset_contains(me, &num));
    num = 2;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 1);
    assert(!unordered_multiset_contains(me, &num));
    num = 5;
    assert(unordered_multiset_remove(me, &num));
    assert(unordered_multiset_size(me) == 0);
    assert(!unordered_multiset_contains(me, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        unordered_multiset_put(me, &i);
        assert(unordered_multiset_contains(me, &i));
    }
    assert(unordered_multiset_size(me) == 1000);
    for (int i = 5000; i < 6000; i++) {
        unordered_multiset_remove(me, &i);
        assert(!unordered_multiset_contains(me, &i));
    }
    assert(unordered_multiset_size(me) == 0);
    assert(unordered_multiset_is_empty(me));
    unordered_multiset_clear(me);
    assert(unordered_multiset_size(me) == 0);
    assert(unordered_multiset_is_empty(me));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        unordered_multiset_put(me, &i);
        assert(unordered_multiset_contains(me, &i));
    }
    assert(unordered_multiset_size(me) == 1000);
    hash_count = 0;
    unordered_multiset_rehash(me);
    assert(hash_count == 1000);
    unordered_multiset_clear(me);
    key = 0xdeadbeef;
    assert(!unordered_multiset_remove(me, &key));
    assert(unordered_multiset_size(me) == 0);
    assert(unordered_multiset_is_empty(me));
    key = 5;
    unordered_multiset_put(me, &key);
    assert(unordered_multiset_size(me) == 1);
    key = 7;
    for (int i = 0; i < 10; i++) {
        unordered_multiset_put(me, &key);
    }
    assert(unordered_multiset_size(me) == 11);
    unordered_multiset_remove_all(me, &key);
    assert(unordered_multiset_size(me) == 1);
    me = unordered_multiset_destroy(me);
    assert(!me);
}
