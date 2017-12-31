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
    multiset a = multiset_init(sizeof(int), compare_int);
    assert(multiset_size(a) == 0);
    assert(multiset_is_empty(a));
    int b = 4;
    multiset_put(a, &b);
    assert(multiset_size(a) == 1);
    multiset_put(a, &b);
    assert(multiset_size(a) == 2);
    assert(multiset_count(a, &b) == 2);
    assert(!multiset_is_empty(a));
    assert(multiset_contains(a, &b));
    b = 7;
    assert(!multiset_contains(a, &b));
    multiset_put(a, &b);
    assert(multiset_size(a) == 3);
    assert(multiset_contains(a, &b));
    multiset_remove(a, &b);
    assert(multiset_size(a) == 2);
    assert(!multiset_contains(a, &b));
    b = 4;
    multiset_remove(a, &b);
    assert(multiset_size(a) == 1);
    multiset_remove(a, &b);
    assert(multiset_size(a) == 0);
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        multiset_put(a, &c[i]);
        assert(multiset_contains(a, &c[i]));
    }
    assert(multiset_size(a) == 10);
    for (int i = 0; i < 10; i++) {
        assert(multiset_contains(a, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(multiset_contains(a, &i) == contains);
    }
    int num = -3;
    assert(!multiset_remove(a, &num));
    assert(multiset_size(a) == 10);
    assert(!multiset_contains(a, &num));
    num = 6;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 9);
    assert(!multiset_contains(a, &num));
    num = 4;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 8);
    assert(!multiset_contains(a, &num));
    num = 7;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 7);
    assert(!multiset_contains(a, &num));
    num = 9;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 6);
    assert(!multiset_contains(a, &num));
    num = -5;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 5);
    assert(!multiset_contains(a, &num));
    num = 0;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 4);
    assert(!multiset_contains(a, &num));
    num = 1;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 3);
    assert(!multiset_contains(a, &num));
    num = 5;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 2);
    assert(multiset_contains(a, &num));
    num = 2;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 1);
    assert(!multiset_contains(a, &num));
    num = 5;
    assert(multiset_remove(a, &num));
    assert(multiset_size(a) == 0);
    assert(!multiset_contains(a, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        multiset_put(a, &i);
        assert(multiset_contains(a, &i));
    }
    assert(multiset_size(a) == 1000);
    for (int i = 5000; i < 6000; i++) {
        multiset_remove(a, &i);
        assert(!multiset_contains(a, &i));
    }
    assert(multiset_size(a) == 0);
    assert(multiset_is_empty(a));
    multiset_clear(a);
    assert(multiset_size(a) == 0);
    assert(multiset_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        multiset_put(a, &i);
        assert(multiset_contains(a, &i));
    }
    assert(multiset_size(a) == 1000);
    multiset_clear(a);
    int p = 0xdeadbeef;
    assert(!multiset_remove(a, &p));
    assert(multiset_size(a) == 0);
    assert(multiset_is_empty(a));
    int m = 5;
    multiset_put(a, &m);
    assert(multiset_size(a) == 1);
    m = 7;
    for (int i = 0; i < 10; i++) {
        multiset_put(a, &m);
    }
    assert(multiset_size(a) == 11);
    multiset_remove_all(a, &m);
    assert(multiset_size(a) == 1);
    a = multiset_destroy(a);
    assert(a == NULL);
}
