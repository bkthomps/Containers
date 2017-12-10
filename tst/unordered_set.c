#include "test.h"
#include "../src/unordered_set.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

static unsigned long hash_int(const void *const key)
{
    unsigned long hash = 17;
    hash = 31 * hash + *(int *) key;
    return hash;
}

void test_unordered_set(void)
{
    unordered_set a = unordered_set_init(sizeof(int), hash_int, compare_int);
    assert(unordered_set_size(a) == 0);
    assert(unordered_set_is_empty(a));
    int b = 4;
    unordered_set_add(a, &b);
    assert(unordered_set_size(a) == 1);
    unordered_set_add(a, &b);
    assert(unordered_set_size(a) == 1);
    assert(!unordered_set_is_empty(a));
    assert(unordered_set_contains(a, &b));
    b = 7;
    assert(!unordered_set_contains(a, &b));
    unordered_set_add(a, &b);
    assert(unordered_set_size(a) == 2);
    assert(unordered_set_contains(a, &b));
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        unordered_set_add(a, &c[i]);
        assert(unordered_set_contains(a, &c[i]));
    }
    assert(unordered_set_size(a) == 9);
    for (int i = 0; i < 10; i++) {
        assert(unordered_set_contains(a, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(unordered_set_contains(a, &i) == contains);
    }
    int num = -3;
    assert(!unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 9);
    assert(!unordered_set_contains(a, &num));
    num = 6;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 8);
    assert(!unordered_set_contains(a, &num));
    num = 4;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 7);
    assert(!unordered_set_contains(a, &num));
    num = 7;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 6);
    assert(!unordered_set_contains(a, &num));
    num = 9;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 5);
    assert(!unordered_set_contains(a, &num));
    num = -5;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 4);
    assert(!unordered_set_contains(a, &num));
    num = 0;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 3);
    assert(!unordered_set_contains(a, &num));
    num = 1;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 2);
    assert(!unordered_set_contains(a, &num));
    num = 5;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 1);
    assert(!unordered_set_contains(a, &num));
    num = 2;
    assert(unordered_set_remove(a, &num));
    assert(unordered_set_size(a) == 0);
    assert(!unordered_set_contains(a, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        unordered_set_add(a, &i);
        assert(unordered_set_contains(a, &i));
    }
    assert(unordered_set_size(a) == 1000);
    for (int i = 5000; i < 6000; i++) {
        unordered_set_remove(a, &i);
        assert(!unordered_set_contains(a, &i));
    }
    assert(unordered_set_size(a) == 0);
    assert(unordered_set_is_empty(a));
    unordered_set_clear(a);
    assert(unordered_set_size(a) == 0);
    assert(unordered_set_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        unordered_set_add(a, &i);
        assert(unordered_set_contains(a, &i));
    }
    assert(unordered_set_size(a) == 1000);
    unordered_set_clear(a);
    int p = 0xdeadbeef;
    assert(!unordered_set_remove(a, &p));
    assert(unordered_set_size(a) == 0);
    assert(unordered_set_is_empty(a));
    a = unordered_set_destroy(a);
    assert(a == NULL);
}
