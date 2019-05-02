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
    int val_arr[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    map me;
    int key;
    int value;
    int num;
    int count;
    bool flip;
    int p;
    int i;
    int j;
    assert(!map_init(0, sizeof(int), compare_int));
    assert(!map_init(sizeof(int), 0, compare_int));
    assert(!map_init(sizeof(int), sizeof(int), NULL));
    me = map_init(sizeof(int), sizeof(int), compare_int);
    assert(me);
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    key = 4;
    value = 9;
    map_put(me, &key, &value);
    assert(map_size(me) == 1);
    value = 5;
    map_put(me, &key, &value);
    assert(map_size(me) == 1);
    assert(!map_is_empty(me));
    assert(map_contains(me, &key));
    value = 0xdeadbeef;
    map_get(&value, me, &key);
    assert(value == 5);
    key = 7;
    assert(!map_contains(me, &key));
    map_put(me, &key, &value);
    assert(map_size(me) == 2);
    assert(map_contains(me, &key));
    for (i = 0; i < 10; i++) {
        map_put(me, &val_arr[i], &value);
        assert(map_contains(me, &val_arr[i]));
    }
    assert(map_size(me) == 9);
    for (i = 0; i < 10; i++) {
        assert(map_contains(me, &val_arr[i]));
    }
    for (i = -100; i < 100; i++) {
        bool contains = false;
        for (j = 0; j < 10; j++) {
            if (val_arr[j] == i) {
                contains = true;
            }
        }
        assert(map_contains(me, &i) == contains);
    }
    num = -3;
    assert(!map_remove(me, &num));
    assert(map_size(me) == 9);
    assert(!map_contains(me, &num));
    num = 6;
    assert(map_remove(me, &num));
    assert(map_size(me) == 8);
    assert(!map_contains(me, &num));
    num = 4;
    assert(map_remove(me, &num));
    assert(map_size(me) == 7);
    assert(!map_contains(me, &num));
    num = 7;
    assert(map_remove(me, &num));
    assert(map_size(me) == 6);
    assert(!map_contains(me, &num));
    num = 9;
    assert(map_remove(me, &num));
    assert(map_size(me) == 5);
    assert(!map_contains(me, &num));
    num = -5;
    assert(map_remove(me, &num));
    assert(map_size(me) == 4);
    assert(!map_contains(me, &num));
    num = 0;
    assert(map_remove(me, &num));
    assert(map_size(me) == 3);
    assert(!map_contains(me, &num));
    num = 1;
    assert(map_remove(me, &num));
    assert(map_size(me) == 2);
    assert(!map_contains(me, &num));
    num = 5;
    assert(map_remove(me, &num));
    assert(map_size(me) == 1);
    assert(!map_contains(me, &num));
    num = 2;
    assert(map_remove(me, &num));
    assert(map_size(me) == 0);
    assert(!map_contains(me, &num));
    /* Add a lot of items and remove individually. */
    for (i = 5000; i < 6000; i++) {
        map_put(me, &i, &value);
        assert(map_contains(me, &i));
    }
    assert(map_size(me) == 1000);
    for (i = 5000; i < 6000; i++) {
        map_remove(me, &i);
        assert(!map_contains(me, &i));
    }
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    map_clear(me);
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    /* Add a lot of items and clear. */
    for (i = 5000; i < 6000; i++) {
        map_put(me, &i, &value);
        assert(map_contains(me, &i));
    }
    assert(map_size(me) == 1000);
    map_clear(me);
    key = 0xdeadbeef;
    assert(!map_remove(me, &key));
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    me = map_destroy(me);
    assert(!me);
    me = map_init(sizeof(int), sizeof(int), compare_int);
    assert(me);
    /* left-left */
    key = 5;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 1;
    map_put(me, &key, &num);
    key = 0xdeadbeef;
    map_contains(me, &key);
    map_clear(me);
    /* right-right */
    key = 1;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 5;
    map_put(me, &key, &num);
    key = 0xdeadbeef;
    map_contains(me, &key);
    map_clear(me);
    /* left-right */
    key = 5;
    map_put(me, &key, &num);
    key = 1;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 0xdeadbeef;
    map_contains(me, &key);
    map_clear(me);
    /* right-left */
    key = 1;
    map_put(me, &key, &num);
    key = 5;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 0xdeadbeef;
    map_contains(me, &key);
    map_clear(me);
    /* Two children edge case. */
    key = 8;
    map_put(me, &key, &num);
    key = 5;
    map_put(me, &key, &num);
    key = 11;
    map_put(me, &key, &num);
    key = 2;
    map_put(me, &key, &num);
    key = 6;
    map_put(me, &key, &num);
    key = 10;
    map_put(me, &key, &num);
    key = 15;
    map_put(me, &key, &num);
    key = 1;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 4;
    map_put(me, &key, &num);
    key = 7;
    map_put(me, &key, &num);
    key = 9;
    map_put(me, &key, &num);
    key = 12;
    map_put(me, &key, &num);
    key = 13;
    map_put(me, &key, &num);
    key = 16;
    map_put(me, &key, &num);
    key = 14;
    map_put(me, &key, &num);
    map_clear(me);
    /* Two children edge case. */
    key = 8;
    map_put(me, &key, &num);
    key = 4;
    map_put(me, &key, &num);
    key = 12;
    map_put(me, &key, &num);
    key = 2;
    map_put(me, &key, &num);
    key = 6;
    map_put(me, &key, &num);
    key = 10;
    map_put(me, &key, &num);
    key = 15;
    map_put(me, &key, &num);
    key = 1;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 5;
    map_put(me, &key, &num);
    key = 7;
    map_put(me, &key, &num);
    key = 9;
    map_put(me, &key, &num);
    key = 11;
    map_put(me, &key, &num);
    key = 13;
    map_put(me, &key, &num);
    key = 16;
    map_put(me, &key, &num);
    key = 14;
    map_put(me, &key, &num);
    map_clear(me);
    /* Add a lot of items. */
    count = 0;
    flip = false;
    for (i = 1234; i < 82400; i++) {
        bool is_already_present;
        bool is_now_present;
        key = i % 765;
        is_already_present = map_contains(me, &key);
        map_put(me, &key, &num);
        is_now_present = map_contains(me, &key);
        assert(is_now_present);
        if (!is_already_present && is_now_present) {
            count++;
        }
        if (i == 1857 && !flip) {
            i *= -1;
            flip = true;
        }
    }
    assert(count == map_size(me));
    map_put(me, &key, &num);
    map_destroy(me);
    me = map_init(sizeof(int), sizeof(int), compare_int);
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    key = 4;
    map_put(me, &key, &num);
    assert(map_size(me) == 1);
    map_put(me, &key, &num);
    assert(map_size(me) == 1);
    assert(!map_is_empty(me));
    assert(map_contains(me, &key));
    key = 7;
    assert(!map_contains(me, &key));
    map_put(me, &key, &num);
    assert(map_size(me) == 2);
    assert(map_contains(me, &key));
    for (i = 0; i < 10; i++) {
        map_put(me, &c[i], &num);
        assert(map_contains(me, &c[i]));
    }
    assert(map_size(me) == 9);
    for (i = 0; i < 10; i++) {
        assert(map_contains(me, &c[i]));
    }
    for (i = -100; i < 100; i++) {
        bool contains = false;
        for (j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(map_contains(me, &i) == contains);
    }
    num = -3;
    assert(!map_remove(me, &num));
    assert(map_size(me) == 9);
    assert(!map_contains(me, &num));
    num = 6;
    assert(map_remove(me, &num));
    assert(map_size(me) == 8);
    assert(!map_contains(me, &num));
    num = 4;
    assert(map_remove(me, &num));
    assert(map_size(me) == 7);
    assert(!map_contains(me, &num));
    num = 7;
    assert(map_remove(me, &num));
    assert(map_size(me) == 6);
    assert(!map_contains(me, &num));
    num = 9;
    assert(map_remove(me, &num));
    assert(map_size(me) == 5);
    assert(!map_contains(me, &num));
    num = -5;
    assert(map_remove(me, &num));
    assert(map_size(me) == 4);
    assert(!map_contains(me, &num));
    num = 0;
    assert(map_remove(me, &num));
    assert(map_size(me) == 3);
    assert(!map_contains(me, &num));
    num = 1;
    assert(map_remove(me, &num));
    assert(map_size(me) == 2);
    assert(!map_contains(me, &num));
    num = 5;
    assert(map_remove(me, &num));
    assert(map_size(me) == 1);
    assert(!map_contains(me, &num));
    num = 2;
    assert(map_remove(me, &num));
    assert(map_size(me) == 0);
    assert(!map_contains(me, &num));
    /* Add a lot of items and remove individually. */
    for (i = 5000; i < 6000; i++) {
        map_put(me, &i, &num);
        assert(map_contains(me, &i));
    }
    assert(map_size(me) == 1000);
    for (i = 5000; i < 5500; i++) {
        map_remove(me, &i);
        assert(!map_contains(me, &i));
    }
    assert(map_size(me) == 500);
    assert(!map_is_empty(me));
    map_clear(me);
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    /* Add a lot of items and clear. */
    for (i = 5000; i < 6000; i++) {
        map_put(me, &i, &num);
        assert(map_contains(me, &i));
    }
    assert(map_size(me) == 1000);
    map_clear(me);
    p = 0xdeadbeef;
    assert(!map_remove(me, &p));
    assert(map_size(me) == 0);
    assert(map_is_empty(me));
    me = map_destroy(me);
    assert(!me);
    /* Create odd shape graph. */
    me = map_init(sizeof(int), sizeof(int), compare_int);
    key = 10;
    map_put(me, &key, &num);
    key = 5;
    map_put(me, &key, &num);
    key = 15;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 8;
    map_put(me, &key, &num);
    key = 12;
    map_put(me, &key, &num);
    key = 18;
    map_put(me, &key, &num);
    key = 12;
    map_remove(me, &key);
    key = 5;
    map_remove(me, &key);
    key = 3;
    map_remove(me, &key);
    key = 8;
    map_remove(me, &key);
    map_clear(me);
    /* Allocate many nodes. */
    for (i = 8123; i < 12314; i += 3) {
        map_put(me, &i, &num);
        assert(map_contains(me, &i));
    }
    for (i = 13000; i > 8000; i--) {
        map_remove(me, &i);
        assert(!map_contains(me, &i));
    }
    map_clear(me);
    /* Create another odd shape graph. */
    key = 20;
    map_put(me, &key, &num);
    key = 10;
    map_put(me, &key, &num);
    key = 40;
    map_put(me, &key, &num);
    key = 5;
    map_put(me, &key, &num);
    key = 15;
    map_put(me, &key, &num);
    key = 30;
    map_put(me, &key, &num);
    key = 50;
    map_put(me, &key, &num);
    key = 25;
    map_put(me, &key, &num);
    key = 35;
    map_put(me, &key, &num);
    key = 36;
    map_put(me, &key, &num);
    key = 34;
    map_put(me, &key, &num);
    key = 33;
    map_put(me, &key, &num);
    key = 32;
    map_put(me, &key, &num);
    key = 30;
    map_remove(me, &key);
    key = 32;
    assert(map_contains(me, &key));
    map_clear(me);
    /* One sided tree. */
    key = 10;
    map_put(me, &key, &num);
    key = 9;
    map_put(me, &key, &num);
    key = 8;
    map_put(me, &key, &num);
    key = 7;
    map_put(me, &key, &num);
    key = 8;
    map_remove(me, &key);
    key = 7;
    assert(map_contains(me, &key));
    map_destroy(me);
    /* Replace two sided two children. */
    me = map_init(sizeof(int), sizeof(int), compare_int);
    key = 5;
    map_put(me, &key, &num);
    key = 1;
    map_put(me, &key, &num);
    key = 6;
    map_put(me, &key, &num);
    key = -1;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 7;
    map_put(me, &key, &num);
    key = -2;
    map_put(me, &key, &num);
    key = 0;
    map_put(me, &key, &num);
    key = 2;
    map_put(me, &key, &num);
    key = 4;
    map_put(me, &key, &num);
    key = 1;
    map_remove(me, &key);
    assert(!map_contains(me, &key));
    map_clear(me);
    key = 5;
    map_put(me, &key, &num);
    key = 1;
    map_put(me, &key, &num);
    key = 6;
    map_put(me, &key, &num);
    key = -1;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 7;
    map_put(me, &key, &num);
    key = -2;
    map_put(me, &key, &num);
    key = 0;
    map_put(me, &key, &num);
    key = 4;
    map_put(me, &key, &num);
    key = 1;
    map_remove(me, &key);
    assert(!map_contains(me, &key));
    me = map_destroy(me);
    assert(!me);
    me = map_init(sizeof(int), sizeof(int), compare_int);
    for (i = 4817; i > -2983; i -= 11) {
        map_put(me, &i, &num);
        assert(map_contains(me, &i));
    }
    for (i = -432; i < 3849; i += 7) {
        map_remove(me, &i);
        assert(!map_contains(me, &i));
    }
    map_clear(me);
    key = 5;
    map_put(me, &key, &num);
    key = 7;
    map_put(me, &key, &num);
    key = 5;
    map_remove(me, &key);
    map_clear(me);
    /* Two children edge case other side. */
    key = 8;
    map_put(me, &key, &num);
    key = 4;
    map_put(me, &key, &num);
    key = 12;
    map_put(me, &key, &num);
    key = 2;
    map_put(me, &key, &num);
    key = 6;
    map_put(me, &key, &num);
    key = 10;
    map_put(me, &key, &num);
    key = 16;
    map_put(me, &key, &num);
    key = 1;
    map_put(me, &key, &num);
    key = 3;
    map_put(me, &key, &num);
    key = 5;
    map_put(me, &key, &num);
    key = 7;
    map_put(me, &key, &num);
    key = 9;
    map_put(me, &key, &num);
    key = 11;
    map_put(me, &key, &num);
    key = 15;
    map_put(me, &key, &num);
    key = 17;
    map_put(me, &key, &num);
    key = 13;
    map_put(me, &key, &num);
    map_clear(me);
    assert(!map_get(&value, me, &key));
}
