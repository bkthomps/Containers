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
    int b;
    // left-left
    b = 5;
    set_add(a, &b, __LINE__);
    b = 3;
    set_add(a, &b, __LINE__);
    b = 1;
    set_add(a, &b, __LINE__);
    b = 0xdeadbeef;
    set_contains(a, &b, __LINE__);
    set_clear(a, __LINE__);
    // right-right
    b = 1;
    set_add(a, &b, __LINE__);
    b = 3;
    set_add(a, &b, __LINE__);
    b = 5;
    set_add(a, &b, __LINE__);
    b = 0xdeadbeef;
    set_contains(a, &b, __LINE__);
    set_clear(a, __LINE__);
    // left-right
    b = 5;
    set_add(a, &b, __LINE__);
    b = 1;
    set_add(a, &b, __LINE__);
    b = 3;
    set_add(a, &b, __LINE__);
    b = 0xdeadbeef;
    set_contains(a, &b, __LINE__);
    set_clear(a, __LINE__);
    // right-left
    b = 1;
    set_add(a, &b, __LINE__);
    b = 5;
    set_add(a, &b, __LINE__);
    b = 3;
    set_add(a, &b, __LINE__);
    b = 0xdeadbeef;
    set_contains(a, &b, __LINE__);
    set_clear(a, __LINE__);
    int count = 0;
    bool flip = false;
    for (int i = 1234; i < 82400; i++) {
        int n = i % 765;
        const bool is_already_present = set_contains(a, &n, __LINE__);
        set_add(a, &n, __LINE__);
        const bool is_now_present = set_contains(a, &n, __LINE__);
        assert(is_now_present);
        if (!is_already_present && is_now_present) {
            count++;
        }
        if (i == 1857 && !flip) {
            i *= -1;
            flip = true;
        }
    }
    assert(count == set_size(a));
    set_contains(a, &b, __LINE__);
    set_destroy(a);
    a = set_init(sizeof(int), compare_int);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    b = 4;
    set_add(a, &b, __LINE__);
    assert(set_size(a) == 1);
    set_add(a, &b, __LINE__);
    assert(set_size(a) == 1);
    assert(!set_is_empty(a));
    assert(set_contains(a, &b, __LINE__));
    b = 7;
    assert(!set_contains(a, &b, __LINE__));
    set_add(a, &b, __LINE__);
    assert(set_size(a) == 2);
    assert(set_contains(a, &b, __LINE__));
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        set_add(a, &c[i], __LINE__);
        assert(set_contains(a, &c[i], __LINE__));
    }
    assert(set_size(a) == 9);
    for (int i = 0; i < 10; i++) {
        assert(set_contains(a, &c[i], __LINE__));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(set_contains(a, &i, __LINE__) == contains);
    }
    int num = -3;
    assert(!set_remove(a, &num, __LINE__));
    assert(set_size(a) == 9);
    assert(!set_contains(a, &num, __LINE__));
    num = 6;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 8);
    assert(!set_contains(a, &num, __LINE__));
    num = 4;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 7);
    assert(!set_contains(a, &num, __LINE__));
    num = 7;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 6);
    assert(!set_contains(a, &num, __LINE__));
    num = 9;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 5);
    assert(!set_contains(a, &num, __LINE__));
    num = -5;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 4);
    assert(!set_contains(a, &num, __LINE__));
    num = 0;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 3);
    assert(!set_contains(a, &num, __LINE__));
    num = 1;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 2);
    assert(!set_contains(a, &num, __LINE__));
    num = 5;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 1);
    assert(!set_contains(a, &num, __LINE__));
    num = 2;
    assert(set_remove(a, &num, __LINE__));
    assert(set_size(a) == 0);
    assert(!set_contains(a, &num, __LINE__));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        set_add(a, &i, __LINE__);
        assert(set_contains(a, &i, __LINE__));
    }
    assert(set_size(a) == 1000);
    for (int i = 5000; i < 5500; i++) {
        set_remove(a, &i, __LINE__);
        assert(!set_contains(a, &i, __LINE__));
    }
    assert(set_size(a) == 500);
    assert(!set_is_empty(a));
    set_clear(a, __LINE__);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        set_add(a, &i, __LINE__);
        assert(set_contains(a, &i, __LINE__));
    }
    assert(set_size(a) == 1000);
    set_clear(a, __LINE__);
    int p = 0xdeadbeef;
    assert(!set_remove(a, &p, __LINE__));
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    a = set_destroy(a);
    assert(a == NULL);
    // Create odd shape graph.
    a = set_init(sizeof(int), compare_int);
    int tmp = 10;
    set_add(a, &tmp, __LINE__);
    tmp = 5;
    set_add(a, &tmp, __LINE__);
    tmp = 15;
    set_add(a, &tmp, __LINE__);
    tmp = 3;
    set_add(a, &tmp, __LINE__);
    tmp = 8;
    set_add(a, &tmp, __LINE__);
    tmp = 12;
    set_add(a, &tmp, __LINE__);
    tmp = 18;
    set_add(a, &tmp, __LINE__);
    tmp = 12;
    set_remove(a, &tmp, __LINE__);
    tmp = 5;
    set_remove(a, &tmp, __LINE__);
    tmp = 3;
    set_remove(a, &tmp, __LINE__);
    tmp = 8;
    set_remove(a, &tmp, __LINE__);
    set_clear(a, __LINE__);
    // Allocate many nodes.
    for (int i = 8123; i < 12314; i += 3) {
        set_add(a, &i, __LINE__);
        assert(set_contains(a, &i, __LINE__));
    }
    for (int i = 13000; i > 8000; i--) {
        set_remove(a, &i, __LINE__);
        assert(!set_contains(a, &i, __LINE__));
    }
    set_clear(a, __LINE__);
    // Create another odd shape graph.
    tmp = 20;
    set_add(a, &tmp, __LINE__);
    tmp = 10;
    set_add(a, &tmp, __LINE__);
    tmp = 40;
    set_add(a, &tmp, __LINE__);
    tmp = 5;
    set_add(a, &tmp, __LINE__);
    tmp = 15;
    set_add(a, &tmp, __LINE__);
    tmp = 30;
    set_add(a, &tmp, __LINE__);
    tmp = 50;
    set_add(a, &tmp, __LINE__);
    tmp = 25;
    set_add(a, &tmp, __LINE__);
    tmp = 35;
    set_add(a, &tmp, __LINE__);
    tmp = 36;
    set_add(a, &tmp, __LINE__);
    tmp = 34;
    set_add(a, &tmp, __LINE__);
    tmp = 33;
    set_add(a, &tmp, __LINE__);
    tmp = 32;
    set_add(a, &tmp, __LINE__);
    tmp = 30;
    set_remove(a, &tmp, __LINE__);
    tmp = 32;
    assert(set_contains(a, &tmp, __LINE__));
    set_clear(a, __LINE__);
    // One sided tree.
    tmp = 10;
    set_add(a, &tmp, __LINE__);
    tmp = 9;
    set_add(a, &tmp, __LINE__);
    tmp = 8;
    set_add(a, &tmp, __LINE__);
    tmp = 7;
    set_add(a, &tmp, __LINE__);
    tmp = 8;
    set_remove(a, &tmp, __LINE__);
    tmp = 7;
    assert(set_contains(a, &tmp, __LINE__));
    set_destroy(a);
    // Replace two sided two children
    a = set_init(sizeof(int), compare_int);
    b = 5;
    set_add(a, &b, __LINE__);
    b = 1;
    set_add(a, &b, __LINE__);
    b = 6;
    set_add(a, &b, __LINE__);
    b = -1;
    set_add(a, &b, __LINE__);
    b = 3;
    set_add(a, &b, __LINE__);
    b = 7;
    set_add(a, &b, __LINE__);
    b = -2;
    set_add(a, &b, __LINE__);
    b = 0;
    set_add(a, &b, __LINE__);
    b = 2;
    set_add(a, &b, __LINE__);
    b = 4;
    set_add(a, &b, __LINE__);
    b = 1;
    set_remove(a, &b, __LINE__);
    assert(!set_contains(a, &b, __LINE__));
    set_clear(a, __LINE__);
    b = 5;
    set_add(a, &b, __LINE__);
    b = 1;
    set_add(a, &b, __LINE__);
    b = 6;
    set_add(a, &b, __LINE__);
    b = -1;
    set_add(a, &b, __LINE__);
    b = 3;
    set_add(a, &b, __LINE__);
    b = 7;
    set_add(a, &b, __LINE__);
    b = -2;
    set_add(a, &b, __LINE__);
    b = 0;
    set_add(a, &b, __LINE__);
    b = 4;
    set_add(a, &b, __LINE__);
    b = 1;
    set_remove(a, &b, __LINE__);
    assert(!set_contains(a, &b, __LINE__));
    set_destroy(a);
}
