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
    multimap a = multimap_init(sizeof(int), sizeof(int),
                               compare_int, compare_int);
    assert(multimap_size(a) == 0);
    assert(multimap_is_empty(a));
    int r = 123;
    int b = 4;
    multimap_put(a, &b, &r);
    assert(multimap_size(a) == 1);
    multimap_put(a, &b, &r);
    assert(multimap_size(a) == 2);
    assert(!multimap_is_empty(a));
    assert(multimap_contains(a, &b));
    b = 7;
    assert(!multimap_contains(a, &b));
    multimap_put(a, &b, &r);
    assert(multimap_size(a) == 3);
    assert(multimap_contains(a, &b));
    multimap_remove(a, &b, &r);
    assert(multimap_size(a) == 2);
    assert(!multimap_contains(a, &b));
    b = 4;
    multimap_remove(a, &b, &r);
    assert(multimap_size(a) == 1);
    multimap_remove(a, &b, &r);
    assert(multimap_size(a) == 0);
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        multimap_put(a, &c[i], &r);
        assert(multimap_contains(a, &c[i]));
    }
    assert(multimap_size(a) == 10);
    for (int i = 0; i < 10; i++) {
        assert(multimap_contains(a, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(multimap_contains(a, &i) == contains);
    }
    int num = -3;
    assert(!multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 10);
    assert(!multimap_contains(a, &num));
    num = 6;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 9);
    assert(!multimap_contains(a, &num));
    num = 4;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 8);
    assert(!multimap_contains(a, &num));
    num = 7;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 7);
    assert(!multimap_contains(a, &num));
    num = 9;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 6);
    assert(!multimap_contains(a, &num));
    num = -5;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 5);
    assert(!multimap_contains(a, &num));
    num = 0;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 4);
    assert(!multimap_contains(a, &num));
    num = 1;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 3);
    assert(!multimap_contains(a, &num));
    num = 5;
    assert(multimap_count(a, &num) == 2);
    multimap_get_start(a, &num);
    int count = 0;
    int val = 0xdeadbeef;
    while (multimap_get_next(&val, a)) {
        count++;
        assert(val == 123);
        val = 0xdeadbeef;
    }
    assert(count == 2);
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 2);
    assert(multimap_contains(a, &num));
    num = 2;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 1);
    assert(!multimap_contains(a, &num));
    num = 5;
    assert(multimap_remove(a, &num, &r));
    assert(multimap_size(a) == 0);
    assert(!multimap_contains(a, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        multimap_put(a, &i, &r);
        assert(multimap_contains(a, &i));
    }
    assert(multimap_size(a) == 1000);
    for (int i = 5000; i < 6000; i++) {
        multimap_remove(a, &i, &r);
        assert(!multimap_contains(a, &i));
    }
    assert(multimap_size(a) == 0);
    assert(multimap_is_empty(a));
    multimap_clear(a);
    assert(multimap_size(a) == 0);
    assert(multimap_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        multimap_put(a, &i, &r);
        assert(multimap_contains(a, &i));
    }
    assert(multimap_size(a) == 1000);
    multimap_clear(a);
    int p = 0xdeadbeef;
    assert(!multimap_remove(a, &p, &r));
    assert(multimap_size(a) == 0);
    assert(multimap_is_empty(a));
    int m = 5;
    multimap_put(a, &m, &r);
    assert(multimap_size(a) == 1);
    m = 7;
    for (int i = 0; i < 10; i++) {
        multimap_put(a, &m, &r);
    }
    assert(multimap_size(a) == 11);
    multimap_remove_all(a, &m);
    assert(multimap_size(a) == 1);
    a = multimap_destroy(a);
    assert(a == NULL);
}
