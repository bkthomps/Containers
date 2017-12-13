#include "test.h"
#include "../src/unordered_multimap.h"

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

void test_unordered_multimap(void)
{
    unordered_multimap a =
            unordered_multimap_init(sizeof(int), sizeof(int), hash_int,
                                    compare_int, compare_int);
    assert(unordered_multimap_size(a) == 0);
    assert(unordered_multimap_is_empty(a));
    int r = 123;
    int b = 4;
    unordered_multimap_put(a, &b, &r);
    assert(unordered_multimap_size(a) == 1);
    unordered_multimap_put(a, &b, &r);
    assert(unordered_multimap_size(a) == 2);
    assert(!unordered_multimap_is_empty(a));
    assert(unordered_multimap_contains(a, &b));
    b = 7;
    assert(!unordered_multimap_contains(a, &b));
    unordered_multimap_put(a, &b, &r);
    assert(unordered_multimap_size(a) == 3);
    assert(unordered_multimap_contains(a, &b));
    unordered_multimap_remove(a, &b, &r);
    assert(unordered_multimap_size(a) == 2);
    assert(!unordered_multimap_contains(a, &b));
    b = 4;
    unordered_multimap_remove(a, &b, &r);
    assert(unordered_multimap_size(a) == 1);
    unordered_multimap_remove(a, &b, &r);
    assert(unordered_multimap_size(a) == 0);
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        unordered_multimap_put(a, &c[i], &r);
        assert(unordered_multimap_contains(a, &c[i]));
    }
    assert(unordered_multimap_size(a) == 10);
    for (int i = 0; i < 10; i++) {
        assert(unordered_multimap_contains(a, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(unordered_multimap_contains(a, &i) == contains);
    }
    int num = -3;
    assert(!unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 10);
    assert(!unordered_multimap_contains(a, &num));
    num = 6;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 9);
    assert(!unordered_multimap_contains(a, &num));
    num = 4;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 8);
    assert(!unordered_multimap_contains(a, &num));
    num = 7;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 7);
    assert(!unordered_multimap_contains(a, &num));
    num = 9;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 6);
    assert(!unordered_multimap_contains(a, &num));
    num = -5;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 5);
    assert(!unordered_multimap_contains(a, &num));
    num = 0;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 4);
    assert(!unordered_multimap_contains(a, &num));
    num = 1;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 3);
    assert(!unordered_multimap_contains(a, &num));
    num = 5;
    assert(unordered_multimap_count(a, &num) == 2);
    unordered_multimap_get_start(a, &num);
    int count = 0;
    int val = 0xdeadbeef;
    while (unordered_multimap_get_next(&val, a)) {
        count++;
        assert(val == 123);
        val = 0xdeadbeef;
    }
    assert(count == 2);
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 2);
    assert(unordered_multimap_contains(a, &num));
    num = 2;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 1);
    assert(!unordered_multimap_contains(a, &num));
    num = 5;
    assert(unordered_multimap_remove(a, &num, &r));
    assert(unordered_multimap_size(a) == 0);
    assert(!unordered_multimap_contains(a, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        unordered_multimap_put(a, &i, &r);
        assert(unordered_multimap_contains(a, &i));
    }
    assert(unordered_multimap_size(a) == 1000);
    for (int i = 5000; i < 6000; i++) {
        unordered_multimap_remove(a, &i, &r);
        assert(!unordered_multimap_contains(a, &i));
    }
    assert(unordered_multimap_size(a) == 0);
    assert(unordered_multimap_is_empty(a));
    unordered_multimap_clear(a);
    assert(unordered_multimap_size(a) == 0);
    assert(unordered_multimap_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        unordered_multimap_put(a, &i, &r);
        assert(unordered_multimap_contains(a, &i));
    }
    assert(unordered_multimap_size(a) == 1000);
    hash_count = 0;
    unordered_multimap_rehash(a);
    assert(hash_count == 1000);
    unordered_multimap_clear(a);
    int p = 0xdeadbeef;
    assert(!unordered_multimap_remove(a, &p, &r));
    assert(unordered_multimap_size(a) == 0);
    assert(unordered_multimap_is_empty(a));
    int m = 5;
    unordered_multimap_put(a, &m, &r);
    assert(unordered_multimap_size(a) == 1);
    m = 7;
    for (int i = 0; i < 10; i++) {
        unordered_multimap_put(a, &m, &r);
    }
    assert(unordered_multimap_size(a) == 11);
    unordered_multimap_remove_all(a, &m);
    assert(unordered_multimap_size(a) == 1);
    a = unordered_multimap_destroy(a);
    assert(a == NULL);
}
