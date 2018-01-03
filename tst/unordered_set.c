#include "test.h"
#include "../src/unordered_set.h"

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

void test_unordered_set(void)
{
    unordered_set me = unordered_set_init(sizeof(int), hash_int, compare_int);
    assert(me);
    assert(unordered_set_size(me) == 0);
    assert(unordered_set_is_empty(me));
    int key = 4;
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
    int val_arr[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        unordered_set_put(me, &val_arr[i]);
        assert(unordered_set_contains(me, &val_arr[i]));
    }
    assert(unordered_set_size(me) == 9);
    for (int i = 0; i < 10; i++) {
        assert(unordered_set_contains(me, &val_arr[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (val_arr[j] == i) {
                contains = true;
            }
        }
        assert(unordered_set_contains(me, &i) == contains);
    }
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
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        unordered_set_put(me, &i);
        assert(unordered_set_contains(me, &i));
    }
    assert(unordered_set_size(me) == 1000);
    for (int i = 5000; i < 6000; i++) {
        unordered_set_remove(me, &i);
        assert(!unordered_set_contains(me, &i));
    }
    assert(unordered_set_size(me) == 0);
    assert(unordered_set_is_empty(me));
    unordered_set_clear(me);
    assert(unordered_set_size(me) == 0);
    assert(unordered_set_is_empty(me));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        unordered_set_put(me, &i);
        assert(unordered_set_contains(me, &i));
    }
    assert(unordered_set_size(me) == 1000);
    hash_count = 0;
    unordered_set_rehash(me);
    assert(hash_count == 1000);
    unordered_set_clear(me);
    int p = 0xdeadbeef;
    assert(!unordered_set_remove(me, &p));
    assert(unordered_set_size(me) == 0);
    assert(unordered_set_is_empty(me));
    me = unordered_set_destroy(me);
    assert(!me);
}
