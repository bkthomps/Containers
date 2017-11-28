#include "test.h"
#include "../src/set.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void test_set(void)
{
    set a = set_init(sizeof(int), compare_int);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    int b = 4;
    set_add(a, &b);
    assert(set_size(a) == 1);
    set_add(a, &b);
    assert(set_size(a) == 1);
    assert(!set_is_empty(a));
    assert(set_contains(a, &b));
    b = 7;
    assert(!set_contains(a, &b));
    set_add(a, &b);
    assert(set_size(a) == 2);
    assert(set_contains(a, &b));
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        set_add(a, &c[i]);
        assert(set_contains(a, &c[i]));
    }
    assert(set_size(a) == 9);
    for (int i = 0; i < 10; i++) {
        assert(set_contains(a, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(set_contains(a, &i) == contains);
    }
    int num = -3;
    assert(!set_remove(a, &num));
    assert(set_size(a) == 9);
    assert(!set_contains(a, &num));
    num = 6;
    assert(set_remove(a, &num));
    assert(set_size(a) == 8);
    assert(!set_contains(a, &num));
    num = 4;
    assert(set_remove(a, &num));
    assert(set_size(a) == 7);
    assert(!set_contains(a, &num));
    num = 7;
    assert(set_remove(a, &num));
    assert(set_size(a) == 6);
    assert(!set_contains(a, &num));
    num = 9;
    assert(set_remove(a, &num));
    assert(set_size(a) == 5);
    assert(!set_contains(a, &num));
    num = -5;
    assert(set_remove(a, &num));
    assert(set_size(a) == 4);
    assert(!set_contains(a, &num));
    num = 0;
    assert(set_remove(a, &num));
    assert(set_size(a) == 3);
    assert(!set_contains(a, &num));
    num = 1;
    assert(set_remove(a, &num));
    assert(set_size(a) == 2);
    assert(!set_contains(a, &num));
    num = 5;
    assert(set_remove(a, &num));
    assert(set_size(a) == 1);
    assert(!set_contains(a, &num));
    num = 2;
    assert(set_remove(a, &num));
    assert(set_size(a) == 0);
    assert(!set_contains(a, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        set_add(a, &i);
        assert(set_contains(a, &i));
    }
    assert(set_size(a) == 1000);
    for (int i = 5000; i < 6000; i++) {
        set_remove(a, &i);
        assert(!set_contains(a, &i));
    }
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    set_clear(a);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        set_add(a, &i);
        assert(set_contains(a, &i));
    }
    assert(set_size(a) == 1000);
    set_clear(a);
    int p = 0xdeadbeef;
    assert(!set_remove(a, &p));
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    a = set_destroy(a);
    assert(a == NULL);
}
