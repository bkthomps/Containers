#include "test.h"
#include "../src/set.h"

/*
 * Include this struct for the stubs.
 */
struct internal_set {
    size_t key_size;
    int (*comparator)(const void *const one, const void *const two);
    int size;
    struct node *root;
};

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void test_set(void)
{
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    set me;
    int key;
    int count;
    int flip;
    int i;
    int j;
    int num;
    int p;
    assert(!set_init(0, compare_int));
    assert(!set_init(sizeof(int), NULL));
    me = set_init(sizeof(int), compare_int);
    assert(me);
    /* left-left */
    key = 5;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 0xdeadbeef;
    set_contains(me, &key);
    set_clear(me);
    /* right-right */
    key = 1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 0xdeadbeef;
    set_contains(me, &key);
    set_clear(me);
    /* left-right */
    key = 5;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 0xdeadbeef;
    set_contains(me, &key);
    set_clear(me);
    /* right-left */
    key = 1;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 0xdeadbeef;
    set_contains(me, &key);
    set_clear(me);
    /* Two children edge case. */
    key = 8;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 11;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    key = 6;
    set_put(me, &key);
    key = 10;
    set_put(me, &key);
    key = 15;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    key = 7;
    set_put(me, &key);
    key = 9;
    set_put(me, &key);
    key = 12;
    set_put(me, &key);
    key = 13;
    set_put(me, &key);
    key = 16;
    set_put(me, &key);
    key = 14;
    set_put(me, &key);
    set_clear(me);
    /* Two children edge case. */
    key = 8;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    key = 12;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    key = 6;
    set_put(me, &key);
    key = 10;
    set_put(me, &key);
    key = 15;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 7;
    set_put(me, &key);
    key = 9;
    set_put(me, &key);
    key = 11;
    set_put(me, &key);
    key = 13;
    set_put(me, &key);
    key = 16;
    set_put(me, &key);
    key = 14;
    set_put(me, &key);
    set_clear(me);
    /* Add a lot of items. */
    count = 0;
    flip = 0;
    for (i = 1234; i < 82400; i++) {
        int is_already_present;
        int is_now_present;
        num = i % 765;
        is_already_present = set_contains(me, &num);
        set_put(me, &num);
        is_now_present = set_contains(me, &num);
        assert(is_now_present);
        if (!is_already_present && is_now_present) {
            count++;
        }
        if (i == 1857 && !flip) {
            i *= -1;
            flip = 1;
        }
    }
    assert(count == set_size(me));
    set_contains(me, &key);
    set_destroy(me);
    me = set_init(sizeof(int), compare_int);
    assert(set_size(me) == 0);
    assert(set_is_empty(me));
    key = 4;
    set_put(me, &key);
    assert(set_size(me) == 1);
    set_put(me, &key);
    assert(set_size(me) == 1);
    assert(!set_is_empty(me));
    assert(set_contains(me, &key));
    key = 7;
    assert(!set_contains(me, &key));
    set_put(me, &key);
    assert(set_size(me) == 2);
    assert(set_contains(me, &key));
    for (i = 0; i < 10; i++) {
        set_put(me, &c[i]);
        assert(set_contains(me, &c[i]));
    }
    assert(set_size(me) == 9);
    for (i = 0; i < 10; i++) {
        assert(set_contains(me, &c[i]));
    }
    for (i = -100; i < 100; i++) {
        int contains = 0;
        for (j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = 1;
            }
        }
        assert(set_contains(me, &i) == contains);
    }
    num = -3;
    assert(!set_remove(me, &num));
    assert(set_size(me) == 9);
    assert(!set_contains(me, &num));
    num = 6;
    assert(set_remove(me, &num));
    assert(set_size(me) == 8);
    assert(!set_contains(me, &num));
    num = 4;
    assert(set_remove(me, &num));
    assert(set_size(me) == 7);
    assert(!set_contains(me, &num));
    num = 7;
    assert(set_remove(me, &num));
    assert(set_size(me) == 6);
    assert(!set_contains(me, &num));
    num = 9;
    assert(set_remove(me, &num));
    assert(set_size(me) == 5);
    assert(!set_contains(me, &num));
    num = -5;
    assert(set_remove(me, &num));
    assert(set_size(me) == 4);
    assert(!set_contains(me, &num));
    num = 0;
    assert(set_remove(me, &num));
    assert(set_size(me) == 3);
    assert(!set_contains(me, &num));
    num = 1;
    assert(set_remove(me, &num));
    assert(set_size(me) == 2);
    assert(!set_contains(me, &num));
    num = 5;
    assert(set_remove(me, &num));
    assert(set_size(me) == 1);
    assert(!set_contains(me, &num));
    num = 2;
    assert(set_remove(me, &num));
    assert(set_size(me) == 0);
    assert(!set_contains(me, &num));
    /* Add a lot of items and remove individually. */
    for (i = 5000; i < 6000; i++) {
        set_put(me, &i);
        assert(set_contains(me, &i));
    }
    assert(set_size(me) == 1000);
    for (i = 5000; i < 5500; i++) {
        set_remove(me, &i);
        assert(!set_contains(me, &i));
    }
    assert(set_size(me) == 500);
    assert(!set_is_empty(me));
    set_clear(me);
    assert(set_size(me) == 0);
    assert(set_is_empty(me));
    /* Add a lot of items and clear. */
    for (i = 5000; i < 6000; i++) {
        set_put(me, &i);
        assert(set_contains(me, &i));
    }
    assert(set_size(me) == 1000);
    set_clear(me);
    p = 0xdeadbeef;
    assert(!set_remove(me, &p));
    assert(set_size(me) == 0);
    assert(set_is_empty(me));
    me = set_destroy(me);
    assert(!me);
    /* Create odd shape graph. */
    me = set_init(sizeof(int), compare_int);
    key = 10;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 15;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 8;
    set_put(me, &key);
    key = 12;
    set_put(me, &key);
    key = 18;
    set_put(me, &key);
    key = 12;
    set_remove(me, &key);
    key = 5;
    set_remove(me, &key);
    key = 3;
    set_remove(me, &key);
    key = 8;
    set_remove(me, &key);
    set_clear(me);
    /* Allocate many nodes. */
    for (i = 8123; i < 12314; i += 3) {
        set_put(me, &i);
        assert(set_contains(me, &i));
    }
    for (i = 13000; i > 8000; i--) {
        set_remove(me, &i);
        assert(!set_contains(me, &i));
    }
    set_clear(me);
    /* Create another odd shape graph. */
    key = 20;
    set_put(me, &key);
    key = 10;
    set_put(me, &key);
    key = 40;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 15;
    set_put(me, &key);
    key = 30;
    set_put(me, &key);
    key = 50;
    set_put(me, &key);
    key = 25;
    set_put(me, &key);
    key = 35;
    set_put(me, &key);
    key = 36;
    set_put(me, &key);
    key = 34;
    set_put(me, &key);
    key = 33;
    set_put(me, &key);
    key = 32;
    set_put(me, &key);
    key = 30;
    set_remove(me, &key);
    key = 32;
    assert(set_contains(me, &key));
    set_clear(me);
    /* One sided tree. */
    key = 10;
    set_put(me, &key);
    key = 9;
    set_put(me, &key);
    key = 8;
    set_put(me, &key);
    key = 7;
    set_put(me, &key);
    key = 8;
    set_remove(me, &key);
    key = 7;
    assert(set_contains(me, &key));
    set_destroy(me);
    /* Replace two sided two children. */
    me = set_init(sizeof(int), compare_int);
    key = 5;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 6;
    set_put(me, &key);
    key = -1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 7;
    set_put(me, &key);
    key = -2;
    set_put(me, &key);
    key = 0;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    key = 1;
    set_remove(me, &key);
    assert(!set_contains(me, &key));
    set_clear(me);
    key = 5;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 6;
    set_put(me, &key);
    key = -1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 7;
    set_put(me, &key);
    key = -2;
    set_put(me, &key);
    key = 0;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    key = 1;
    set_remove(me, &key);
    assert(!set_contains(me, &key));
    me = set_destroy(me);
    assert(!me);
    me = set_init(sizeof(int), compare_int);
    for (i = 4817; i > -2983; i -= 11) {
        set_put(me, &i);
        assert(set_contains(me, &i));
    }
    for (i = -432; i < 3849; i += 7) {
        set_remove(me, &i);
        assert(!set_contains(me, &i));
    }
    set_clear(me);
    key = 5;
    set_put(me, &key);
    key = 7;
    set_put(me, &key);
    key = 5;
    set_remove(me, &key);
    set_clear(me);
    /* Two children edge case on the other side. */
    key = 8;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    key = 12;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    key = 6;
    set_put(me, &key);
    key = 10;
    set_put(me, &key);
    key = 16;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 7;
    set_put(me, &key);
    key = 9;
    set_put(me, &key);
    key = 11;
    set_put(me, &key);
    key = 15;
    set_put(me, &key);
    key = 17;
    set_put(me, &key);
    key = 13;
    set_put(me, &key);
    set_clear(me);
}
