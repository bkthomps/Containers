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
    assert(!multiset_init(0, compare_int));
    assert(!multiset_init(sizeof(int), NULL));
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    int key = 4;
    multiset_put(me, &key);
    assert(multiset_size(me) == 1);
    multiset_put(me, &key);
    assert(multiset_size(me) == 2);
    assert(multiset_count(me, &key) == 2);
    assert(!multiset_is_empty(me));
    assert(multiset_contains(me, &key));
    key = 7;
    assert(!multiset_contains(me, &key));
    multiset_put(me, &key);
    assert(multiset_size(me) == 3);
    assert(multiset_contains(me, &key));
    multiset_remove(me, &key);
    assert(multiset_size(me) == 2);
    assert(!multiset_contains(me, &key));
    key = 4;
    multiset_remove(me, &key);
    assert(multiset_size(me) == 1);
    multiset_remove(me, &key);
    assert(multiset_size(me) == 0);
    int val_arr[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        multiset_put(me, &val_arr[i]);
        assert(multiset_contains(me, &val_arr[i]));
    }
    assert(multiset_size(me) == 10);
    for (int i = 0; i < 10; i++) {
        assert(multiset_contains(me, &val_arr[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (val_arr[j] == i) {
                contains = true;
            }
        }
        assert(multiset_contains(me, &i) == contains);
    }
    int num = -3;
    assert(!multiset_remove(me, &num));
    assert(multiset_size(me) == 10);
    assert(!multiset_contains(me, &num));
    num = 6;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 9);
    assert(!multiset_contains(me, &num));
    num = 4;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 8);
    assert(!multiset_contains(me, &num));
    num = 7;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 7);
    assert(!multiset_contains(me, &num));
    num = 9;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 6);
    assert(!multiset_contains(me, &num));
    num = -5;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 5);
    assert(!multiset_contains(me, &num));
    num = 0;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 4);
    assert(!multiset_contains(me, &num));
    num = 1;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 3);
    assert(!multiset_contains(me, &num));
    num = 5;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 2);
    assert(multiset_contains(me, &num));
    num = 2;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 1);
    assert(!multiset_contains(me, &num));
    num = 5;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 0);
    assert(!multiset_contains(me, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 1000);
    for (int i = 5000; i < 6000; i++) {
        multiset_remove(me, &i);
        assert(!multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    multiset_clear(me);
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 1000);
    multiset_clear(me);
    key = 0xdeadbeef;
    assert(!multiset_remove(me, &key));
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    key = 5;
    multiset_put(me, &key);
    assert(multiset_size(me) == 1);
    key = 7;
    for (int i = 0; i < 10; i++) {
        multiset_put(me, &key);
    }
    assert(multiset_size(me) == 11);
    multiset_remove_all(me, &key);
    assert(multiset_size(me) == 1);
    me = multiset_destroy(me);
    assert(!me);
    me = multiset_init(sizeof(int), compare_int);
    assert(me);
    // left-left
    key = 5;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 1;
    multiset_put(me, &key);
    key = 0xdeadbeef;
    multiset_contains(me, &key);
    multiset_clear(me);
    // right-right
    key = 1;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 5;
    multiset_put(me, &key);
    key = 0xdeadbeef;
    multiset_contains(me, &key);
    multiset_clear(me);
    // left-right
    key = 5;
    multiset_put(me, &key);
    key = 1;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 0xdeadbeef;
    multiset_contains(me, &key);
    multiset_clear(me);
    // right-left
    key = 1;
    multiset_put(me, &key);
    key = 5;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 0xdeadbeef;
    multiset_contains(me, &key);
    multiset_clear(me);
    // Two children edge case.
    key = 8;
    multiset_put(me, &key);
    key = 5;
    multiset_put(me, &key);
    key = 11;
    multiset_put(me, &key);
    key = 2;
    multiset_put(me, &key);
    key = 6;
    multiset_put(me, &key);
    key = 10;
    multiset_put(me, &key);
    key = 15;
    multiset_put(me, &key);
    key = 1;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 4;
    multiset_put(me, &key);
    key = 7;
    multiset_put(me, &key);
    key = 9;
    multiset_put(me, &key);
    key = 12;
    multiset_put(me, &key);
    key = 13;
    multiset_put(me, &key);
    key = 16;
    multiset_put(me, &key);
    key = 14;
    multiset_put(me, &key);
    multiset_clear(me);
    // Two children edge case.
    key = 8;
    multiset_put(me, &key);
    key = 4;
    multiset_put(me, &key);
    key = 12;
    multiset_put(me, &key);
    key = 2;
    multiset_put(me, &key);
    key = 6;
    multiset_put(me, &key);
    key = 10;
    multiset_put(me, &key);
    key = 15;
    multiset_put(me, &key);
    key = 1;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 5;
    multiset_put(me, &key);
    key = 7;
    multiset_put(me, &key);
    key = 9;
    multiset_put(me, &key);
    key = 11;
    multiset_put(me, &key);
    key = 13;
    multiset_put(me, &key);
    key = 16;
    multiset_put(me, &key);
    key = 14;
    multiset_put(me, &key);
    multiset_clear(me);
    // Add a lot of items.
    int count = 0;
    bool flip = false;
    for (int i = 1234; i < 82400; i++) {
        key = i % 765;
        const bool is_already_present = multiset_contains(me, &key);
        multiset_put(me, &key);
        const bool is_now_present = multiset_contains(me, &key);
        assert(is_now_present);
        if (!is_already_present && is_now_present) {
            count++;
        }
        if (i == 1857 && !flip) {
            i *= -1;
            flip = true;
        }
    }
    multiset_contains(me, &key);
    multiset_destroy(me);
    me = multiset_init(sizeof(int), compare_int);
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    key = 4;
    multiset_put(me, &key);
    assert(multiset_size(me) == 1);
    multiset_put(me, &key);
    assert(multiset_size(me) == 2);
    assert(!multiset_is_empty(me));
    assert(multiset_contains(me, &key));
    key = 7;
    assert(!multiset_contains(me, &key));
    multiset_put(me, &key);
    assert(multiset_size(me) == 3);
    assert(multiset_contains(me, &key));
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        multiset_put(me, &c[i]);
        assert(multiset_contains(me, &c[i]));
    }
    assert(multiset_size(me) == 13);
    for (int i = 0; i < 10; i++) {
        assert(multiset_contains(me, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(multiset_contains(me, &i) == contains);
    }
    num = -3;
    assert(!multiset_remove(me, &num));
    assert(multiset_size(me) == 13);
    assert(!multiset_contains(me, &num));
    num = 6;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 12);
    assert(!multiset_contains(me, &num));
    num = 4;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 11);
    assert(multiset_contains(me, &num));
    num = 7;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 10);
    assert(multiset_contains(me, &num));
    num = 9;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 9);
    assert(!multiset_contains(me, &num));
    num = -5;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 8);
    assert(!multiset_contains(me, &num));
    num = 0;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 7);
    assert(!multiset_contains(me, &num));
    num = 1;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 6);
    assert(!multiset_contains(me, &num));
    num = 5;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 5);
    assert(multiset_contains(me, &num));
    num = 2;
    assert(multiset_remove(me, &num));
    assert(multiset_size(me) == 4);
    assert(!multiset_contains(me, &num));
    multiset_clear(me);
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 1000);
    for (int i = 5000; i < 5500; i++) {
        multiset_remove(me, &i);
        assert(!multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 500);
    assert(!multiset_is_empty(me));
    multiset_clear(me);
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    assert(multiset_size(me) == 1000);
    multiset_clear(me);
    int p = 0xdeadbeef;
    assert(!multiset_remove(me, &p));
    assert(multiset_size(me) == 0);
    assert(multiset_is_empty(me));
    me = multiset_destroy(me);
    assert(!me);
    // Create odd shape graph.
    me = multiset_init(sizeof(int), compare_int);
    key = 10;
    multiset_put(me, &key);
    key = 5;
    multiset_put(me, &key);
    key = 15;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 8;
    multiset_put(me, &key);
    key = 12;
    multiset_put(me, &key);
    key = 18;
    multiset_put(me, &key);
    key = 12;
    multiset_remove(me, &key);
    key = 5;
    multiset_remove(me, &key);
    key = 3;
    multiset_remove(me, &key);
    key = 8;
    multiset_remove(me, &key);
    multiset_clear(me);
    // Allocate many nodes.
    for (int i = 8123; i < 12314; i += 3) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    for (int i = 13000; i > 8000; i--) {
        multiset_remove(me, &i);
        assert(!multiset_contains(me, &i));
    }
    multiset_clear(me);
    // Create another odd shape graph.
    key = 20;
    multiset_put(me, &key);
    key = 10;
    multiset_put(me, &key);
    key = 40;
    multiset_put(me, &key);
    key = 5;
    multiset_put(me, &key);
    key = 15;
    multiset_put(me, &key);
    key = 30;
    multiset_put(me, &key);
    key = 50;
    multiset_put(me, &key);
    key = 25;
    multiset_put(me, &key);
    key = 35;
    multiset_put(me, &key);
    key = 36;
    multiset_put(me, &key);
    key = 34;
    multiset_put(me, &key);
    key = 33;
    multiset_put(me, &key);
    key = 32;
    multiset_put(me, &key);
    key = 30;
    multiset_remove(me, &key);
    key = 32;
    assert(multiset_contains(me, &key));
    multiset_clear(me);
    // One sided tree.
    key = 10;
    multiset_put(me, &key);
    key = 9;
    multiset_put(me, &key);
    key = 8;
    multiset_put(me, &key);
    key = 7;
    multiset_put(me, &key);
    key = 8;
    multiset_remove(me, &key);
    key = 7;
    assert(multiset_contains(me, &key));
    multiset_destroy(me);
    // Replace two sided two children.
    me = multiset_init(sizeof(int), compare_int);
    key = 5;
    multiset_put(me, &key);
    key = 1;
    multiset_put(me, &key);
    key = 6;
    multiset_put(me, &key);
    key = -1;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 7;
    multiset_put(me, &key);
    key = -2;
    multiset_put(me, &key);
    key = 0;
    multiset_put(me, &key);
    key = 2;
    multiset_put(me, &key);
    key = 4;
    multiset_put(me, &key);
    key = 1;
    multiset_remove(me, &key);
    assert(!multiset_contains(me, &key));
    multiset_clear(me);
    key = 5;
    multiset_put(me, &key);
    key = 1;
    multiset_put(me, &key);
    key = 6;
    multiset_put(me, &key);
    key = -1;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 7;
    multiset_put(me, &key);
    key = -2;
    multiset_put(me, &key);
    key = 0;
    multiset_put(me, &key);
    key = 4;
    multiset_put(me, &key);
    key = 1;
    multiset_remove(me, &key);
    assert(!multiset_contains(me, &key));
    me = multiset_destroy(me);
    assert(!me);
    me = multiset_init(sizeof(int), compare_int);
    for (int i = 4817; i > -2983; i -= 11) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    for (int i = -432; i < 3849; i += 7) {
        multiset_remove(me, &i);
        assert(!multiset_contains(me, &i));
    }
    multiset_clear(me);
    key = 5;
    multiset_put(me, &key);
    key = 7;
    multiset_put(me, &key);
    key = 5;
    multiset_remove(me, &key);
    multiset_clear(me);
    // Two children edge case other side.
    key = 8;
    multiset_put(me, &key);
    key = 4;
    multiset_put(me, &key);
    key = 12;
    multiset_put(me, &key);
    key = 2;
    multiset_put(me, &key);
    key = 6;
    multiset_put(me, &key);
    key = 10;
    multiset_put(me, &key);
    key = 16;
    multiset_put(me, &key);
    key = 1;
    multiset_put(me, &key);
    key = 3;
    multiset_put(me, &key);
    key = 5;
    multiset_put(me, &key);
    key = 7;
    multiset_put(me, &key);
    key = 9;
    multiset_put(me, &key);
    key = 11;
    multiset_put(me, &key);
    key = 15;
    multiset_put(me, &key);
    key = 17;
    multiset_put(me, &key);
    key = 13;
    multiset_put(me, &key);
    multiset_clear(me);
    assert(multiset_count(me, &key) == 0);
    assert(!multiset_remove_all(me, &key));
}
