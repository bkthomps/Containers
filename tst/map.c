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
    map a = map_init(sizeof(int), sizeof(int), compare_int);
    assert(map_size(a) == 0);
    assert(map_is_empty(a));
    int b = 4;
    int c = 9;
    map_put(a, &b, &c);
    assert(map_size(a) == 1);
    c = 5;
    map_put(a, &b, &c);
    assert(map_size(a) == 1);
    assert(!map_is_empty(a));
    assert(map_contains(a, &b));
    c = 0xdeadbeef;
    map_get(&c, a, &b);
    assert(c == 5);
    b = 7;
    assert(!map_contains(a, &b));
    map_put(a, &b, &c);
    assert(map_size(a) == 2);
    assert(map_contains(a, &b));
    int d[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        map_put(a, &d[i], &c);
        assert(map_contains(a, &d[i]));
    }
    assert(map_size(a) == 9);
    for (int i = 0; i < 10; i++) {
        assert(map_contains(a, &d[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (d[j] == i) {
                contains = true;
            }
        }
        assert(map_contains(a, &i) == contains);
    }
    int num = -3;
    assert(!map_remove(a, &num));
    assert(map_size(a) == 9);
    assert(!map_contains(a, &num));
    num = 6;
    assert(map_remove(a, &num));
    assert(map_size(a) == 8);
    assert(!map_contains(a, &num));
    num = 4;
    assert(map_remove(a, &num));
    assert(map_size(a) == 7);
    assert(!map_contains(a, &num));
    num = 7;
    assert(map_remove(a, &num));
    assert(map_size(a) == 6);
    assert(!map_contains(a, &num));
    num = 9;
    assert(map_remove(a, &num));
    assert(map_size(a) == 5);
    assert(!map_contains(a, &num));
    num = -5;
    assert(map_remove(a, &num));
    assert(map_size(a) == 4);
    assert(!map_contains(a, &num));
    num = 0;
    assert(map_remove(a, &num));
    assert(map_size(a) == 3);
    assert(!map_contains(a, &num));
    num = 1;
    assert(map_remove(a, &num));
    assert(map_size(a) == 2);
    assert(!map_contains(a, &num));
    num = 5;
    assert(map_remove(a, &num));
    assert(map_size(a) == 1);
    assert(!map_contains(a, &num));
    num = 2;
    assert(map_remove(a, &num));
    assert(map_size(a) == 0);
    assert(!map_contains(a, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        map_put(a, &i, &c);
        assert(map_contains(a, &i));
    }
    assert(map_size(a) == 1000);
    for (int i = 5000; i < 6000; i++) {
        map_remove(a, &i);
        assert(!map_contains(a, &i));
    }
    assert(map_size(a) == 0);
    assert(map_is_empty(a));
    map_clear(a);
    assert(map_size(a) == 0);
    assert(map_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        map_put(a, &i, &c);
        assert(map_contains(a, &i));
    }
    assert(map_size(a) == 1000);
    map_clear(a);
    int p = 0xdeadbeef;
    assert(!map_remove(a, &p));
    assert(map_size(a) == 0);
    assert(map_is_empty(a));
    a = map_destroy(a);
    assert(a == NULL);
}
