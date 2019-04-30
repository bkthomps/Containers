#include "test.h"
#include "../src/set.h"

// Used for thorough testing, but takes longer to run.
//#define LONG_TEST

/*
 * Include this struct for the stubs.
 */
struct internal_set {
    size_t key_size;
    int (*comparator)(const void *const one, const void *const two);
    int size;
    struct node *root;
};

/*
 * Include this struct for the stubs.
 */
struct node {
    struct node *parent;
    int balance;
    void *key;
    struct node *left;
    struct node *right;
};

/*
 * Verifies that the AVL tree rules are followed. The balance factor of an item
 * must be the right height minus the left height. Also, the left key must be
 * less than the right key.
 */
static int set_verify_recursive(struct node *const item)
{
    if (!item) {
        return 0;
    }
    const int left = set_verify_recursive(item->left);
    const int right = set_verify_recursive(item->right);
    const int max = left > right ? left : right;
    assert(right - left == item->balance);
    if (item->left && item->right) {
        const int left_val = *(int *) item->left->key;
        const int right_val = *(int *) item->right->key;
        assert(left_val < right_val);
    }
    if (item->left) {
        assert(item->left->parent == item);
        assert(item->left->parent->key == item->key);
    }
    if (item->right) {
        assert(item->right->parent == item);
        assert(item->right->parent->key == item->key);
    }
    return max + 1;
}

static int set_compute_size(struct node *const item)
{
    if (!item) {
        return 0;
    }
    return 1 + set_compute_size(item->left) + set_compute_size(item->right);
}

static void set_verify(set me)
{
#ifdef LONG_TEST
    set_verify_recursive(me->root);
    assert(set_compute_size(me->root) == set_size(me));
#endif
}

static int stub_set_put(set me, void *const key)
{
    const int ret = set_put(me, key);
    set_verify(me);
    return ret;
}

static bool stub_set_contains(set me, void *const key)
{
    const bool ret = set_contains(me, key);
    set_verify(me);
    return ret;
}

static bool stub_set_remove(set me, void *const key)
{
    const bool ret = set_remove(me, key);
    set_verify(me);
    return ret;
}

static void stub_set_clear(set me)
{
    set_clear(me);
    set_verify(me);
}

static set stub_set_destroy(set me)
{
    set ret = set_destroy(me);
    set_verify(me);
    return ret;
}

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void test_set(void)
{
    assert(!set_init(0, compare_int));
    assert(!set_init(sizeof(int), NULL));
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    int key;
    // left-left
    key = 5;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 1;
    stub_set_put(me, &key);
    key = 0xdeadbeef;
    stub_set_contains(me, &key);
    stub_set_clear(me);
    // right-right
    key = 1;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 5;
    stub_set_put(me, &key);
    key = 0xdeadbeef;
    stub_set_contains(me, &key);
    stub_set_clear(me);
    // left-right
    key = 5;
    stub_set_put(me, &key);
    key = 1;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 0xdeadbeef;
    stub_set_contains(me, &key);
    stub_set_clear(me);
    // right-left
    key = 1;
    stub_set_put(me, &key);
    key = 5;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 0xdeadbeef;
    stub_set_contains(me, &key);
    stub_set_clear(me);
    // Two children edge case.
    key = 8;
    stub_set_put(me, &key);
    key = 5;
    stub_set_put(me, &key);
    key = 11;
    stub_set_put(me, &key);
    key = 2;
    stub_set_put(me, &key);
    key = 6;
    stub_set_put(me, &key);
    key = 10;
    stub_set_put(me, &key);
    key = 15;
    stub_set_put(me, &key);
    key = 1;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 4;
    stub_set_put(me, &key);
    key = 7;
    stub_set_put(me, &key);
    key = 9;
    stub_set_put(me, &key);
    key = 12;
    stub_set_put(me, &key);
    key = 13;
    stub_set_put(me, &key);
    key = 16;
    stub_set_put(me, &key);
    key = 14;
    stub_set_put(me, &key);
    stub_set_clear(me);
    // Two children edge case.
    key = 8;
    stub_set_put(me, &key);
    key = 4;
    stub_set_put(me, &key);
    key = 12;
    stub_set_put(me, &key);
    key = 2;
    stub_set_put(me, &key);
    key = 6;
    stub_set_put(me, &key);
    key = 10;
    stub_set_put(me, &key);
    key = 15;
    stub_set_put(me, &key);
    key = 1;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 5;
    stub_set_put(me, &key);
    key = 7;
    stub_set_put(me, &key);
    key = 9;
    stub_set_put(me, &key);
    key = 11;
    stub_set_put(me, &key);
    key = 13;
    stub_set_put(me, &key);
    key = 16;
    stub_set_put(me, &key);
    key = 14;
    stub_set_put(me, &key);
    stub_set_clear(me);
    // Add a lot of items.
    int count = 0;
    bool flip = false;
    for (int i = 1234; i < 82400; i++) {
        int num = i % 765;
        const bool is_already_present = stub_set_contains(me, &num);
        stub_set_put(me, &num);
        const bool is_now_present = stub_set_contains(me, &num);
        assert(is_now_present);
        if (!is_already_present && is_now_present) {
            count++;
        }
        if (i == 1857 && !flip) {
            i *= -1;
            flip = true;
        }
    }
    assert(count == set_size(me));
    stub_set_contains(me, &key);
    stub_set_destroy(me);
    me = set_init(sizeof(int), compare_int);
    assert(set_size(me) == 0);
    assert(set_is_empty(me));
    key = 4;
    stub_set_put(me, &key);
    assert(set_size(me) == 1);
    stub_set_put(me, &key);
    assert(set_size(me) == 1);
    assert(!set_is_empty(me));
    assert(stub_set_contains(me, &key));
    key = 7;
    assert(!stub_set_contains(me, &key));
    stub_set_put(me, &key);
    assert(set_size(me) == 2);
    assert(stub_set_contains(me, &key));
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        stub_set_put(me, &c[i]);
        assert(stub_set_contains(me, &c[i]));
    }
    assert(set_size(me) == 9);
    for (int i = 0; i < 10; i++) {
        assert(stub_set_contains(me, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(stub_set_contains(me, &i) == contains);
    }
    int num = -3;
    assert(!stub_set_remove(me, &num));
    assert(set_size(me) == 9);
    assert(!stub_set_contains(me, &num));
    num = 6;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 8);
    assert(!stub_set_contains(me, &num));
    num = 4;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 7);
    assert(!stub_set_contains(me, &num));
    num = 7;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 6);
    assert(!stub_set_contains(me, &num));
    num = 9;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 5);
    assert(!stub_set_contains(me, &num));
    num = -5;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 4);
    assert(!stub_set_contains(me, &num));
    num = 0;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 3);
    assert(!stub_set_contains(me, &num));
    num = 1;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 2);
    assert(!stub_set_contains(me, &num));
    num = 5;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 1);
    assert(!stub_set_contains(me, &num));
    num = 2;
    assert(stub_set_remove(me, &num));
    assert(set_size(me) == 0);
    assert(!stub_set_contains(me, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        stub_set_put(me, &i);
        assert(stub_set_contains(me, &i));
    }
    assert(set_size(me) == 1000);
    for (int i = 5000; i < 5500; i++) {
        stub_set_remove(me, &i);
        assert(!stub_set_contains(me, &i));
    }
    assert(set_size(me) == 500);
    assert(!set_is_empty(me));
    stub_set_clear(me);
    assert(set_size(me) == 0);
    assert(set_is_empty(me));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        stub_set_put(me, &i);
        assert(stub_set_contains(me, &i));
    }
    assert(set_size(me) == 1000);
    stub_set_clear(me);
    int p = 0xdeadbeef;
    assert(!stub_set_remove(me, &p));
    assert(set_size(me) == 0);
    assert(set_is_empty(me));
    me = stub_set_destroy(me);
    assert(!me);
    // Create odd shape graph.
    me = set_init(sizeof(int), compare_int);
    key = 10;
    stub_set_put(me, &key);
    key = 5;
    stub_set_put(me, &key);
    key = 15;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 8;
    stub_set_put(me, &key);
    key = 12;
    stub_set_put(me, &key);
    key = 18;
    stub_set_put(me, &key);
    key = 12;
    stub_set_remove(me, &key);
    key = 5;
    stub_set_remove(me, &key);
    key = 3;
    stub_set_remove(me, &key);
    key = 8;
    stub_set_remove(me, &key);
    stub_set_clear(me);
    // Allocate many nodes.
    for (int i = 8123; i < 12314; i += 3) {
        stub_set_put(me, &i);
        assert(stub_set_contains(me, &i));
    }
    for (int i = 13000; i > 8000; i--) {
        stub_set_remove(me, &i);
        assert(!stub_set_contains(me, &i));
    }
    stub_set_clear(me);
    // Create another odd shape graph.
    key = 20;
    stub_set_put(me, &key);
    key = 10;
    stub_set_put(me, &key);
    key = 40;
    stub_set_put(me, &key);
    key = 5;
    stub_set_put(me, &key);
    key = 15;
    stub_set_put(me, &key);
    key = 30;
    stub_set_put(me, &key);
    key = 50;
    stub_set_put(me, &key);
    key = 25;
    stub_set_put(me, &key);
    key = 35;
    stub_set_put(me, &key);
    key = 36;
    stub_set_put(me, &key);
    key = 34;
    stub_set_put(me, &key);
    key = 33;
    stub_set_put(me, &key);
    key = 32;
    stub_set_put(me, &key);
    key = 30;
    stub_set_remove(me, &key);
    key = 32;
    assert(stub_set_contains(me, &key));
    stub_set_clear(me);
    // One sided tree.
    key = 10;
    stub_set_put(me, &key);
    key = 9;
    stub_set_put(me, &key);
    key = 8;
    stub_set_put(me, &key);
    key = 7;
    stub_set_put(me, &key);
    key = 8;
    stub_set_remove(me, &key);
    key = 7;
    assert(stub_set_contains(me, &key));
    stub_set_destroy(me);
    // Replace two sided two children.
    me = set_init(sizeof(int), compare_int);
    key = 5;
    stub_set_put(me, &key);
    key = 1;
    stub_set_put(me, &key);
    key = 6;
    stub_set_put(me, &key);
    key = -1;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 7;
    stub_set_put(me, &key);
    key = -2;
    stub_set_put(me, &key);
    key = 0;
    stub_set_put(me, &key);
    key = 2;
    stub_set_put(me, &key);
    key = 4;
    stub_set_put(me, &key);
    key = 1;
    stub_set_remove(me, &key);
    assert(!stub_set_contains(me, &key));
    stub_set_clear(me);
    key = 5;
    stub_set_put(me, &key);
    key = 1;
    stub_set_put(me, &key);
    key = 6;
    stub_set_put(me, &key);
    key = -1;
    stub_set_put(me, &key);
    key = 3;
    stub_set_put(me, &key);
    key = 7;
    stub_set_put(me, &key);
    key = -2;
    stub_set_put(me, &key);
    key = 0;
    stub_set_put(me, &key);
    key = 4;
    stub_set_put(me, &key);
    key = 1;
    stub_set_remove(me, &key);
    assert(!stub_set_contains(me, &key));
    me = stub_set_destroy(me);
    assert(!me);
    me = set_init(sizeof(int), compare_int);
    for (int i = 4817; i > -2983; i -= 11) {
        stub_set_put(me, &i);
        assert(stub_set_contains(me, &i));
    }
    for (int i = -432; i < 3849; i += 7) {
        stub_set_remove(me, &i);
        assert(!stub_set_contains(me, &i));
    }
    stub_set_clear(me);
    key = 5;
    stub_set_put(me, &key);
    key = 7;
    stub_set_put(me, &key);
    key = 5;
    stub_set_remove(me, &key);
}
