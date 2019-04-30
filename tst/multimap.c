#include "test.h"
#include "../src/multimap.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void test_multimap(void)
{
    assert(!multimap_init(0, sizeof(int), compare_int, compare_int));
    assert(!multimap_init(sizeof(int), 0, compare_int, compare_int));
    assert(!multimap_init(sizeof(int), sizeof(int), NULL, compare_int));
    assert(!multimap_init(sizeof(int), sizeof(int), compare_int, NULL));
    multimap me = multimap_init(sizeof(int), sizeof(int),
                                compare_int, compare_int);
    assert(me);
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    int key = 4;
    int value = 123;
    multimap_put(me, &key, &value);
    assert(multimap_size(me) == 1);
    multimap_put(me, &key, &value);
    assert(multimap_size(me) == 2);
    assert(!multimap_is_empty(me));
    assert(multimap_contains(me, &key));
    key = 7;
    assert(!multimap_contains(me, &key));
    multimap_put(me, &key, &value);
    assert(multimap_size(me) == 3);
    assert(multimap_contains(me, &key));
    multimap_remove(me, &key, &value);
    assert(multimap_size(me) == 2);
    assert(!multimap_contains(me, &key));
    key = 4;
    multimap_remove(me, &key, &value);
    assert(multimap_size(me) == 1);
    multimap_remove(me, &key, &value);
    assert(multimap_size(me) == 0);
    int val_arr[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        multimap_put(me, &val_arr[i], &value);
        assert(multimap_contains(me, &val_arr[i]));
    }
    assert(multimap_size(me) == 10);
    for (int i = 0; i < 10; i++) {
        assert(multimap_contains(me, &val_arr[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (val_arr[j] == i) {
                contains = true;
            }
        }
        assert(multimap_contains(me, &i) == contains);
    }
    int num = -3;
    assert(!multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 10);
    assert(!multimap_contains(me, &num));
    num = 6;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 9);
    assert(!multimap_contains(me, &num));
    num = 4;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 8);
    assert(!multimap_contains(me, &num));
    num = 7;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 7);
    assert(!multimap_contains(me, &num));
    num = 9;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 6);
    assert(!multimap_contains(me, &num));
    num = -5;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 5);
    assert(!multimap_contains(me, &num));
    num = 0;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 4);
    assert(!multimap_contains(me, &num));
    num = 1;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 3);
    assert(!multimap_contains(me, &num));
    num = 5;
    assert(multimap_count(me, &num) == 2);
    multimap_get_start(me, &num);
    int count = 0;
    int val = 0xdeadbeef;
    while (multimap_get_next(&val, me)) {
        count++;
        assert(val == 123);
        val = 0xdeadbeef;
    }
    assert(count == 2);
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 2);
    assert(multimap_contains(me, &num));
    num = 2;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 1);
    assert(!multimap_contains(me, &num));
    num = 5;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 0);
    assert(!multimap_contains(me, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        multimap_put(me, &i, &value);
        assert(multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 1000);
    for (int i = 5000; i < 6000; i++) {
        multimap_remove(me, &i, &value);
        assert(!multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    multimap_clear(me);
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        multimap_put(me, &i, &value);
        assert(multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 1000);
    multimap_clear(me);
    key = 0xdeadbeef;
    assert(!multimap_remove(me, &key, &value));
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    key = 5;
    multimap_put(me, &key, &value);
    assert(multimap_size(me) == 1);
    key = 7;
    for (int i = 0; i < 10; i++) {
        multimap_put(me, &key, &value);
    }
    assert(multimap_size(me) == 11);
    multimap_remove_all(me, &key);
    assert(multimap_size(me) == 1);
    me = multimap_destroy(me);
    assert(!me);
}
