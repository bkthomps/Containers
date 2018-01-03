#include "test.h"
#include "../src/multiset.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void test_multiset(void)
{
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    int key = 4;
    multiset_put(me, &key);
    assert(multiset_size(me) == 1);
    multiset_put(me, &key);
    assert(multiset_size(me) == 2);
    assert(multiset_count(me, &key) == 2);
    assert(!multiset_is_empty(me));
    assert(multiset_contains(me, &key));
    key = 7;
    assert(!multiset_contains(me, &key));
    multiset_put(me, &key);
    assert(multiset_size(me) == 3);
    assert(multiset_contains(me, &key));
    multiset_remove(me, &key);
    assert(multiset_size(me) == 2);
    assert(!multiset_contains(me, &key));
    key = 4;
    multiset_remove(me, &key);
    assert(multiset_size(me) == 1);
    multiset_remove(me, &key);
    assert(multiset_size(me) == 0);
    int val_arr[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        multiset_put(me, &val_arr[i]);
        assert(multiset_contains(me, &val_arr[i]));
    }
    assert(multiset_size(me) == 10);
    for (int i = 0; i < 10; i++) {
        assert(multiset_contains(me, &val_arr[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (val_arr[j] == i) {
                contains = true;
            }
        }
        assert(multiset_contains(me, &i) == contains);
    }
    int num = -3;
    assert(!multiset_remove(me, &num));
    assert(multiset_size(me) == 10);
    assert(!multiset_contains(me, &num));
    num = 6;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 9);
    assert(!multiset_contains(me, &num));
    num = 4;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 8);
    assert(!multiset_contains(me, &num));
    num = 7;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 7);
    assert(!multiset_contains(me, &num));
    num = 9;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 6);
    assert(!multiset_contains(me, &num));
    num = -5;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 5);
    assert(!multiset_contains(me, &num));
    num = 0;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 4);
    assert(!multiset_contains(me, &num));
    num = 1;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 3);
    assert(!multiset_contains(me, &num));
    num = 5;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 2);
    assert(multiset_contains(me, &num));
    num = 2;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 1);
    assert(!multiset_contains(me, &num));
    num = 5;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 0);
    assert(!multiset_contains(me, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 1000);
    for (int i = 5000; i < 6000; i++) {
        multiset_remove(me, &i);
        assert(!multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    multiset_clear(me);
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 1000);
    multiset_clear(me);
    key = 0xdeadbeef;
    assert(!multiset_remove(me, &key));
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    key = 5;
    multiset_put(me, &key);
    assert(multiset_size(me) == 1);
    key = 7;
    for (int i = 0; i < 10; i++) {
        multiset_put(me, &key);
    }
    assert(multiset_size(me) == 11);
    multiset_remove_all(me, &key);
    assert(multiset_size(me) == 1);
    me = multiset_destroy(me);
    assert(!me);
}
