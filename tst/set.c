#include "test.h"
#include "../src/set.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void set_dump(set me);
void set_depth(set me);

void test_set(void)
{
    set a = set_init(sizeof(int), compare_int);
    int b;
    // left-left
    b = 5;
    set_add(a, &b);
    b = 3;
    set_add(a, &b);
    b = 1;
    set_add(a, &b);
    b = 0xdeadbeef;
    set_contains(a, &b);
    set_clear(a);
    // right-right
    b = 1;
    set_add(a, &b);
    b = 3;
    set_add(a, &b);
    b = 5;
    set_add(a, &b);
    b = 0xdeadbeef;
    set_contains(a, &b);
    set_clear(a);
    // left-right
    b = 5;
    set_add(a, &b);
    b = 1;
    set_add(a, &b);
    b = 3;
    set_add(a, &b);
    b = 0xdeadbeef;
    set_contains(a, &b);
    set_clear(a);
    // right-left
    b = 1;
    set_add(a, &b);
    b = 5;
    set_add(a, &b);
    b = 3;
    set_add(a, &b);
    b = 0xdeadbeef;
    set_contains(a, &b);
    set_clear(a);
    int count = 0;
    bool flip = false;
    for (int i = 1234; i < 82400; i++) {
        int n = i % 765;
        const bool is_already_present = set_contains(a, &n);
        set_add(a, &n);
        const bool is_now_present = set_contains(a, &n);
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
    set_contains(a, &b);
    set_destroy(a);
    a = set_init(sizeof(int), compare_int);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    b = 4;
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
    for (int i = 5000; i < 5500; i++) {
        set_remove(a, &i);
        assert(!set_contains(a, &i));
    }
    assert(set_size(a) == 500);
    assert(!set_is_empty(a));
    set_depth(a);
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
    // Create odd shape graph.
    a = set_init(sizeof(int), compare_int);
    int tmp = 10;
    set_add(a, &tmp);
    tmp = 5;
    set_add(a, &tmp);
    tmp = 15;
    set_add(a, &tmp);
    tmp = 3;
    set_add(a, &tmp);
    tmp = 8;
    set_add(a, &tmp);
    tmp = 12;
    set_add(a, &tmp);
    tmp = 18;
    set_add(a, &tmp);
    tmp = 12;
    set_remove(a, &tmp);
    tmp = 5;
    set_remove(a, &tmp);
    tmp = 3;
    set_remove(a, &tmp);
    tmp = 8;
    set_remove(a, &tmp);
    set_clear(a);
    // Allocate many nodes.
    for (int i = 8123; i < 12314; i += 3) {
        set_add(a, &i);
        assert(set_contains(a, &i));
    }
    for (int i = 13000; i > 8000; i--) {
        set_remove(a, &i);
        assert(!set_contains(a, &i));
    }
    set_clear(a);
    // Create another odd shape graph.
    tmp = 20;
    set_add(a, &tmp);
    tmp = 10;
    set_add(a, &tmp);
    tmp = 40;
    set_add(a, &tmp);
    tmp = 5;
    set_add(a, &tmp);
    tmp = 15;
    set_add(a, &tmp);
    tmp = 30;
    set_add(a, &tmp);
    tmp = 50;
    set_add(a, &tmp);
    tmp = 25;
    set_add(a, &tmp);
    tmp = 35;
    set_add(a, &tmp);
    tmp = 36;
    set_add(a, &tmp);
    tmp = 34;
    set_add(a, &tmp);
    tmp = 33;
    set_add(a, &tmp);
    tmp = 32;
    set_add(a, &tmp);
    tmp = 30;
    set_remove(a, &tmp);
    tmp = 32;
    assert(set_contains(a, &tmp));
    set_clear(a);
    // One sided tree.
    tmp = 10;
    set_add(a, &tmp);
    tmp = 9;
    set_add(a, &tmp);
    tmp = 8;
    set_add(a, &tmp);
    tmp = 7;
    set_add(a, &tmp);
    tmp = 8;
    set_remove(a, &tmp);
    tmp = 7;
    assert(set_contains(a, &tmp));
    set_destroy(a);
}
