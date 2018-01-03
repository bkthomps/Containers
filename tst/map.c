#include "test.h"
#include "../src/map.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void test_map(void)
{
    map me = map_init(sizeof(int), sizeof(int), compare_int);
    assert(me);
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    int key = 4;
    int value = 9;
    map_put(me, &key, &value);
    assert(map_size(me) == 1);
    value = 5;
    map_put(me, &key, &value);
    assert(map_size(me) == 1);
    assert(!map_is_empty(me));
    assert(map_contains(me, &key));
    value = 0xdeadbeef;
    map_get(&value, me, &key);
    assert(value == 5);
    key = 7;
    assert(!map_contains(me, &key));
    map_put(me, &key, &value);
    assert(map_size(me) == 2);
    assert(map_contains(me, &key));
    int val_arr[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        map_put(me, &val_arr[i], &value);
        assert(map_contains(me, &val_arr[i]));
    }
    assert(map_size(me) == 9);
    for (int i = 0; i < 10; i++) {
        assert(map_contains(me, &val_arr[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (val_arr[j] == i) {
                contains = true;
            }
        }
        assert(map_contains(me, &i) == contains);
    }
    int num = -3;
    assert(!map_remove(me, &num));
    assert(map_size(me) == 9);
    assert(!map_contains(me, &num));
    num = 6;
    assert(map_remove(me, &num));
    assert(map_size(me) == 8);
    assert(!map_contains(me, &num));
    num = 4;
    assert(map_remove(me, &num));
    assert(map_size(me) == 7);
    assert(!map_contains(me, &num));
    num = 7;
    assert(map_remove(me, &num));
    assert(map_size(me) == 6);
    assert(!map_contains(me, &num));
    num = 9;
    assert(map_remove(me, &num));
    assert(map_size(me) == 5);
    assert(!map_contains(me, &num));
    num = -5;
    assert(map_remove(me, &num));
    assert(map_size(me) == 4);
    assert(!map_contains(me, &num));
    num = 0;
    assert(map_remove(me, &num));
    assert(map_size(me) == 3);
    assert(!map_contains(me, &num));
    num = 1;
    assert(map_remove(me, &num));
    assert(map_size(me) == 2);
    assert(!map_contains(me, &num));
    num = 5;
    assert(map_remove(me, &num));
    assert(map_size(me) == 1);
    assert(!map_contains(me, &num));
    num = 2;
    assert(map_remove(me, &num));
    assert(map_size(me) == 0);
    assert(!map_contains(me, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        map_put(me, &i, &value);
        assert(map_contains(me, &i));
    }
    assert(map_size(me) == 1000);
    for (int i = 5000; i < 6000; i++) {
        map_remove(me, &i);
        assert(!map_contains(me, &i));
    }
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    map_clear(me);
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        map_put(me, &i, &value);
        assert(map_contains(me, &i));
    }
    assert(map_size(me) == 1000);
    map_clear(me);
    key = 0xdeadbeef;
    assert(!map_remove(me, &key));
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    me = map_destroy(me);
    assert(!me);
}
