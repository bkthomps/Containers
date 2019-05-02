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
    int val_arr[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    multimap me;
    int key;
    int value;
    int num;
    int count;
    int val;
    bool flip;
    int p;
    int i;
    int j;
    assert(!multimap_init(0, sizeof(int), compare_int, compare_int));
    assert(!multimap_init(sizeof(int), 0, compare_int, compare_int));
    assert(!multimap_init(sizeof(int), sizeof(int), NULL, compare_int));
    assert(!multimap_init(sizeof(int), sizeof(int), compare_int, NULL));
    me = multimap_init(sizeof(int), sizeof(int), compare_int, compare_int);
    assert(me);
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    key = 4;
    value = 123;
    multimap_put(me, &key, &value);
    assert(multimap_size(me) == 1);
    multimap_put(me, &key, &value);
    assert(multimap_size(me) == 2);
    assert(!multimap_is_empty(me));
    assert(multimap_contains(me, &key));
    key = 7;
    assert(!multimap_contains(me, &key));
    multimap_put(me, &key, &value);
    assert(multimap_size(me) == 3);
    assert(multimap_contains(me, &key));
    multimap_remove(me, &key, &value);
    assert(multimap_size(me) == 2);
    assert(!multimap_contains(me, &key));
    key = 4;
    multimap_remove(me, &key, &value);
    assert(multimap_size(me) == 1);
    multimap_remove(me, &key, &value);
    assert(multimap_size(me) == 0);

    for (i = 0; i < 10; i++) {
        multimap_put(me, &val_arr[i], &value);
        assert(multimap_contains(me, &val_arr[i]));
    }
    assert(multimap_size(me) == 10);
    for (i = 0; i < 10; i++) {
        assert(multimap_contains(me, &val_arr[i]));
    }
    for (i = -100; i < 100; i++) {
        bool contains = false;
        for (j = 0; j < 10; j++) {
            if (val_arr[j] == i) {
                contains = true;
            }
        }
        assert(multimap_contains(me, &i) == contains);
    }
    num = -3;
    assert(!multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 10);
    assert(!multimap_contains(me, &num));
    num = 6;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 9);
    assert(!multimap_contains(me, &num));
    num = 4;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 8);
    assert(!multimap_contains(me, &num));
    num = 7;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 7);
    assert(!multimap_contains(me, &num));
    num = 9;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 6);
    assert(!multimap_contains(me, &num));
    num = -5;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 5);
    assert(!multimap_contains(me, &num));
    num = 0;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 4);
    assert(!multimap_contains(me, &num));
    num = 1;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 3);
    assert(!multimap_contains(me, &num));
    num = 5;
    assert(multimap_count(me, &num) == 2);
    multimap_get_start(me, &num);
    count = 0;
    val = 0xdeadbeef;
    while (multimap_get_next(&val, me)) {
        count++;
        assert(val == 123);
        val = 0xdeadbeef;
    }
    assert(count == 2);
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 2);
    assert(multimap_contains(me, &num));
    num = 2;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 1);
    assert(!multimap_contains(me, &num));
    num = 5;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 0);
    assert(!multimap_contains(me, &num));
    // Add a lot of items and remove individually.
    for (i = 5000; i < 6000; i++) {
        multimap_put(me, &i, &value);
        assert(multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 1000);
    for (i = 5000; i < 6000; i++) {
        multimap_remove(me, &i, &value);
        assert(!multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    multimap_clear(me);
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    // Add a lot of items and clear.
    for (i = 5000; i < 6000; i++) {
        multimap_put(me, &i, &value);
        assert(multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 1000);
    multimap_clear(me);
    key = 0xdeadbeef;
    assert(!multimap_remove(me, &key, &value));
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    key = 5;
    multimap_put(me, &key, &value);
    assert(multimap_size(me) == 1);
    key = 7;
    for (i = 0; i < 10; i++) {
        multimap_put(me, &key, &value);
    }
    assert(multimap_size(me) == 11);
    multimap_remove_all(me, &key);
    assert(multimap_size(me) == 1);
    me = multimap_destroy(me);
    assert(!me);
    me = multimap_init(sizeof(int), sizeof(int), compare_int, compare_int);
    assert(me);
    // left-left
    key = 5;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_put(me, &key, &num);
    key = 0xdeadbeef;
    multimap_contains(me, &key);
    multimap_clear(me);
    // right-right
    key = 1;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 5;
    multimap_put(me, &key, &num);
    key = 0xdeadbeef;
    multimap_contains(me, &key);
    multimap_clear(me);
    // left-right
    key = 5;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 0xdeadbeef;
    multimap_contains(me, &key);
    multimap_clear(me);
    // right-left
    key = 1;
    multimap_put(me, &key, &num);
    key = 5;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 0xdeadbeef;
    multimap_contains(me, &key);
    multimap_clear(me);
    // Two children edge case.
    key = 8;
    multimap_put(me, &key, &num);
    key = 5;
    multimap_put(me, &key, &num);
    key = 11;
    multimap_put(me, &key, &num);
    key = 2;
    multimap_put(me, &key, &num);
    key = 6;
    multimap_put(me, &key, &num);
    key = 10;
    multimap_put(me, &key, &num);
    key = 15;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 4;
    multimap_put(me, &key, &num);
    key = 7;
    multimap_put(me, &key, &num);
    key = 9;
    multimap_put(me, &key, &num);
    key = 12;
    multimap_put(me, &key, &num);
    key = 13;
    multimap_put(me, &key, &num);
    key = 16;
    multimap_put(me, &key, &num);
    key = 14;
    multimap_put(me, &key, &num);
    multimap_clear(me);
    // Two children edge case.
    key = 8;
    multimap_put(me, &key, &num);
    key = 4;
    multimap_put(me, &key, &num);
    key = 12;
    multimap_put(me, &key, &num);
    key = 2;
    multimap_put(me, &key, &num);
    key = 6;
    multimap_put(me, &key, &num);
    key = 10;
    multimap_put(me, &key, &num);
    key = 15;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 5;
    multimap_put(me, &key, &num);
    key = 7;
    multimap_put(me, &key, &num);
    key = 9;
    multimap_put(me, &key, &num);
    key = 11;
    multimap_put(me, &key, &num);
    key = 13;
    multimap_put(me, &key, &num);
    key = 16;
    multimap_put(me, &key, &num);
    key = 14;
    multimap_put(me, &key, &num);
    multimap_clear(me);
    // Add a lot of items.
    count = 0;
    flip = false;
    for (i = 1234; i < 82400; i++) {
        key = i % 765;
        multimap_put(me, &key, &num);
        assert(multimap_contains(me, &key));
        count++;
        if (i == 1857 && !flip) {
            i *= -1;
            flip = true;
        }
    }
    assert(count == multimap_size(me));
    multimap_put(me, &key, &num);
    multimap_destroy(me);
    me = multimap_init(sizeof(int), sizeof(int), compare_int, compare_int);
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    key = 4;
    multimap_put(me, &key, &num);
    assert(multimap_size(me) == 1);
    multimap_put(me, &key, &num);
    assert(multimap_size(me) == 2);
    assert(!multimap_is_empty(me));
    assert(multimap_contains(me, &key));
    key = 7;
    assert(!multimap_contains(me, &key));
    multimap_put(me, &key, &num);
    assert(multimap_size(me) == 3);
    assert(multimap_contains(me, &key));
    for (i = 0; i < 10; i++) {
        multimap_put(me, &c[i], &value);
        assert(multimap_contains(me, &c[i]));
    }
    assert(multimap_size(me) == 13);
    for (i = 0; i < 10; i++) {
        assert(multimap_contains(me, &c[i]));
    }
    for (i = -100; i < 100; i++) {
        bool contains = false;
        for (j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(multimap_contains(me, &i) == contains);
    }
    num = -3;
    assert(!multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 13);
    assert(!multimap_contains(me, &num));
    num = 6;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 12);
    assert(!multimap_contains(me, &num));
    num = 4;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 11);
    assert(multimap_contains(me, &num));
    num = 7;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 10);
    assert(multimap_contains(me, &num));
    num = 9;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 9);
    assert(!multimap_contains(me, &num));
    num = -5;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 8);
    assert(!multimap_contains(me, &num));
    num = 0;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 7);
    assert(!multimap_contains(me, &num));
    num = 1;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 6);
    assert(!multimap_contains(me, &num));
    num = 5;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 5);
    assert(multimap_contains(me, &num));
    num = 2;
    assert(multimap_remove(me, &num, &value));
    assert(multimap_size(me) == 4);
    assert(!multimap_contains(me, &num));
    multimap_clear(me);
    // Add a lot of items and remove individually.
    value = 37;
    for (i = 5000; i < 6000; i++) {
        multimap_put(me, &i, &value);
        assert(multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 1000);
    for (i = 5000; i < 5500; i++) {
        multimap_remove(me, &i, &value);
        assert(!multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 500);
    assert(!multimap_is_empty(me));
    multimap_clear(me);
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    // Add a lot of items and clear.
    for (i = 5000; i < 6000; i++) {
        multimap_put(me, &i, &num);
        assert(multimap_contains(me, &i));
    }
    assert(multimap_size(me) == 1000);
    multimap_clear(me);
    p = 0xdeadbeef;
    assert(!multimap_remove(me, &p, &value));
    assert(multimap_size(me) == 0);
    assert(multimap_is_empty(me));
    me = multimap_destroy(me);
    assert(!me);
    // Create odd shape graph.
    me = multimap_init(sizeof(int), sizeof(int), compare_int, compare_int);
    key = 10;
    multimap_put(me, &key, &num);
    key = 5;
    multimap_put(me, &key, &num);
    key = 15;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 8;
    multimap_put(me, &key, &num);
    key = 12;
    multimap_put(me, &key, &num);
    key = 18;
    multimap_put(me, &key, &num);
    key = 12;
    multimap_remove(me, &key, &value);
    key = 5;
    multimap_remove(me, &key, &value);
    key = 3;
    multimap_remove(me, &key, &value);
    key = 8;
    multimap_remove(me, &key, &value);
    multimap_clear(me);
    // Allocate many nodes.
    value = 54;
    for (i = 8123; i < 12314; i += 3) {
        multimap_put(me, &i, &value);
        assert(multimap_contains(me, &i));
    }
    for (i = 13000; i > 8000; i--) {
        multimap_remove(me, &i, &value);
        assert(!multimap_contains(me, &i));
    }
    multimap_clear(me);
    // Create another odd shape graph.
    key = 20;
    multimap_put(me, &key, &num);
    key = 10;
    multimap_put(me, &key, &num);
    key = 40;
    multimap_put(me, &key, &num);
    key = 5;
    multimap_put(me, &key, &num);
    key = 15;
    multimap_put(me, &key, &num);
    key = 30;
    multimap_put(me, &key, &num);
    key = 50;
    multimap_put(me, &key, &num);
    key = 25;
    multimap_put(me, &key, &num);
    key = 35;
    multimap_put(me, &key, &num);
    key = 36;
    multimap_put(me, &key, &num);
    key = 34;
    multimap_put(me, &key, &num);
    key = 33;
    multimap_put(me, &key, &num);
    key = 32;
    multimap_put(me, &key, &num);
    key = 30;
    multimap_remove(me, &key, &value);
    key = 32;
    assert(multimap_contains(me, &key));
    multimap_clear(me);
    // One sided tree.
    key = 10;
    multimap_put(me, &key, &num);
    key = 9;
    multimap_put(me, &key, &num);
    key = 8;
    multimap_put(me, &key, &num);
    key = 7;
    multimap_put(me, &key, &num);
    key = 8;
    multimap_remove(me, &key, &value);
    key = 7;
    assert(multimap_contains(me, &key));
    multimap_destroy(me);
    // Replace two sided two children.
    me = multimap_init(sizeof(int), sizeof(int), compare_int, compare_int);
    key = 5;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_put(me, &key, &num);
    key = 6;
    multimap_put(me, &key, &num);
    key = -1;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 7;
    multimap_put(me, &key, &num);
    key = -2;
    multimap_put(me, &key, &num);
    key = 0;
    multimap_put(me, &key, &num);
    key = 2;
    multimap_put(me, &key, &num);
    key = 4;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_remove_all(me, &key);
    assert(!multimap_contains(me, &key));
    multimap_clear(me);
    key = 5;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_put(me, &key, &num);
    key = 6;
    multimap_put(me, &key, &num);
    key = -1;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 7;
    multimap_put(me, &key, &num);
    key = -2;
    multimap_put(me, &key, &num);
    key = 0;
    multimap_put(me, &key, &num);
    key = 4;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_remove_all(me, &key);
    assert(!multimap_contains(me, &key));
    me = multimap_destroy(me);
    assert(!me);
    me = multimap_init(sizeof(int), sizeof(int), compare_int, compare_int);
    value = 97;
    for (i = 4817; i > -2983; i -= 11) {
        multimap_put(me, &i, &value);
        assert(multimap_contains(me, &i));
    }
    for (i = -432; i < 3849; i += 7) {
        multimap_remove(me, &i, &value);
        assert(!multimap_contains(me, &i));
    }
    multimap_clear(me);
    key = 5;
    multimap_put(me, &key, &num);
    key = 7;
    multimap_put(me, &key, &num);
    key = 5;
    multimap_remove(me, &key, &value);
    multimap_clear(me);
    // Two children edge case other side.
    key = 8;
    multimap_put(me, &key, &num);
    key = 4;
    multimap_put(me, &key, &num);
    key = 12;
    multimap_put(me, &key, &num);
    key = 2;
    multimap_put(me, &key, &num);
    key = 6;
    multimap_put(me, &key, &num);
    key = 10;
    multimap_put(me, &key, &num);
    key = 16;
    multimap_put(me, &key, &num);
    key = 1;
    multimap_put(me, &key, &num);
    key = 3;
    multimap_put(me, &key, &num);
    key = 5;
    multimap_put(me, &key, &num);
    key = 7;
    multimap_put(me, &key, &num);
    key = 9;
    multimap_put(me, &key, &num);
    key = 11;
    multimap_put(me, &key, &num);
    key = 15;
    multimap_put(me, &key, &num);
    key = 17;
    multimap_put(me, &key, &num);
    key = 13;
    multimap_put(me, &key, &num);
    multimap_clear(me);
    assert(multimap_count(me, &key) == 0);
    assert(!multimap_remove_all(me, &key));
    // Edge case.
    me = multimap_init(sizeof(int), sizeof(int), compare_int, compare_int);
    value = 17;
    key = 5;
    multimap_put(me, &key, &value);
    key = 3;
    multimap_put(me, &key, &value);
    key = 7;
    multimap_put(me, &key, &value);
    key = 2;
    multimap_put(me, &key, &value);
    key = 6;
    multimap_put(me, &key, &value);
    key = 9;
    multimap_put(me, &key, &value);
    key = 3;
    multimap_remove(me, &key, &value);
}
