#include "test.h"
#include "../src/set.h"

#include <stdio.h>

// Used for thorough testing, but takes longer to run.
//#define LONG_TEST

/*
 * Include this struct for the stubs.
 */
struct _set {
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
    if (item == NULL) {
        return 0;
    }
    const int left = set_verify_recursive(item->left);
    const int right = set_verify_recursive(item->right);
    const int max = left > right ? left : right;
    assert(right - left == item->balance);
    if (item->left != NULL && item->right != NULL) {
        const int left_val = *(int *) item->left->key;
        const int right_val = *(int *) item->right->key;
        assert(left_val < right_val);
    }
    if (item->left != NULL) {
        assert(item->left->parent == item);
        assert(item->left->parent->key == item->key);
    }
    if (item->right != NULL) {
        assert(item->right->parent == item);
        assert(item->right->parent->key == item->key);
    }
    return max + 1;
}

static int set_compute_size(struct node *const item)
{
    if (item == NULL) {
        return 0;
    }
    return 1 + set_compute_size(item->left) + set_compute_size(item->right);
}

void set_verify(set me)
{
#ifdef LONG_TEST
    set_verify_recursive(me->root);
    assert(set_compute_size(me->root) == set_size(me));
#endif
}

static int stub_set_add(set me, void *key, const int line)
{
    printf("add %d\n", line);
    const int ret = set_add(me, key);
    set_verify(me);
    return ret;
}

static bool stub_set_contains(set me, void *key, const int line)
{
    printf("contains %d\n", line);
    const bool ret = set_contains(me, key);
    set_verify(me);
    return ret;
}

static bool stub_set_remove(set me, void *key, const int line)
{
    printf("remove %d\n", line);
    const bool ret = set_remove(me, key);
    set_verify(me);
    return ret;
}

static void stub_set_clear(set me, const int line)
{
    printf("clear %d\n", line);
    set_clear(me);
    set_verify(me);
}

static set stub_set_destroy(set me, const int line)
{
    printf("destroy %d\n", line);
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
    set a = set_init(sizeof(int), compare_int);
    int b;
    // left-left
    b = 5;
    stub_set_add(a, &b, __LINE__);
    b = 3;
    stub_set_add(a, &b, __LINE__);
    b = 1;
    stub_set_add(a, &b, __LINE__);
    b = 0xdeadbeef;
    stub_set_contains(a, &b, __LINE__);
    stub_set_clear(a, __LINE__);
    // right-right
    b = 1;
    stub_set_add(a, &b, __LINE__);
    b = 3;
    stub_set_add(a, &b, __LINE__);
    b = 5;
    stub_set_add(a, &b, __LINE__);
    b = 0xdeadbeef;
    stub_set_contains(a, &b, __LINE__);
    stub_set_clear(a, __LINE__);
    // left-right
    b = 5;
    stub_set_add(a, &b, __LINE__);
    b = 1;
    stub_set_add(a, &b, __LINE__);
    b = 3;
    stub_set_add(a, &b, __LINE__);
    b = 0xdeadbeef;
    stub_set_contains(a, &b, __LINE__);
    stub_set_clear(a, __LINE__);
    // right-left
    b = 1;
    stub_set_add(a, &b, __LINE__);
    b = 5;
    stub_set_add(a, &b, __LINE__);
    b = 3;
    stub_set_add(a, &b, __LINE__);
    b = 0xdeadbeef;
    stub_set_contains(a, &b, __LINE__);
    stub_set_clear(a, __LINE__);
    int count = 0;
    bool flip = false;
    for (int i = 1234; i < 82400; i++) {
        int n = i % 765;
        const bool is_already_present = stub_set_contains(a, &n, __LINE__);
        stub_set_add(a, &n, __LINE__);
        const bool is_now_present = stub_set_contains(a, &n, __LINE__);
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
    stub_set_contains(a, &b, __LINE__);
    stub_set_destroy(a, __LINE__);
    a = set_init(sizeof(int), compare_int);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    b = 4;
    stub_set_add(a, &b, __LINE__);
    assert(set_size(a) == 1);
    stub_set_add(a, &b, __LINE__);
    assert(set_size(a) == 1);
    assert(!set_is_empty(a));
    assert(stub_set_contains(a, &b, __LINE__));
    b = 7;
    assert(!stub_set_contains(a, &b, __LINE__));
    stub_set_add(a, &b, __LINE__);
    assert(set_size(a) == 2);
    assert(stub_set_contains(a, &b, __LINE__));
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        stub_set_add(a, &c[i], __LINE__);
        assert(stub_set_contains(a, &c[i], __LINE__));
    }
    assert(set_size(a) == 9);
    for (int i = 0; i < 10; i++) {
        assert(stub_set_contains(a, &c[i], __LINE__));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(stub_set_contains(a, &i, __LINE__) == contains);
    }
    int num = -3;
    assert(!stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 9);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = 6;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 8);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = 4;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 7);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = 7;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 6);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = 9;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 5);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = -5;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 4);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = 0;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 3);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = 1;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 2);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = 5;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 1);
    assert(!stub_set_contains(a, &num, __LINE__));
    num = 2;
    assert(stub_set_remove(a, &num, __LINE__));
    assert(set_size(a) == 0);
    assert(!stub_set_contains(a, &num, __LINE__));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        stub_set_add(a, &i, __LINE__);
        assert(stub_set_contains(a, &i, __LINE__));
    }
    assert(set_size(a) == 1000);
    for (int i = 5000; i < 5500; i++) {
        stub_set_remove(a, &i, __LINE__);
        assert(!stub_set_contains(a, &i, __LINE__));
    }
    assert(set_size(a) == 500);
    assert(!set_is_empty(a));
    stub_set_clear(a, __LINE__);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        stub_set_add(a, &i, __LINE__);
        assert(stub_set_contains(a, &i, __LINE__));
    }
    assert(set_size(a) == 1000);
    stub_set_clear(a, __LINE__);
    int p = 0xdeadbeef;
    assert(!stub_set_remove(a, &p, __LINE__));
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    a = stub_set_destroy(a, __LINE__);
    assert(a == NULL);
    // Create odd shape graph.
    a = set_init(sizeof(int), compare_int);
    int tmp = 10;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 5;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 15;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 3;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 8;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 12;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 18;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 12;
    stub_set_remove(a, &tmp, __LINE__);
    tmp = 5;
    stub_set_remove(a, &tmp, __LINE__);
    tmp = 3;
    stub_set_remove(a, &tmp, __LINE__);
    tmp = 8;
    stub_set_remove(a, &tmp, __LINE__);
    stub_set_clear(a, __LINE__);
    // Allocate many nodes.
    for (int i = 8123; i < 12314; i += 3) {
        stub_set_add(a, &i, __LINE__);
        assert(stub_set_contains(a, &i, __LINE__));
    }
    for (int i = 13000; i > 8000; i--) {
        stub_set_remove(a, &i, __LINE__);
        assert(!stub_set_contains(a, &i, __LINE__));
    }
    stub_set_clear(a, __LINE__);
    // Create another odd shape graph.
    tmp = 20;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 10;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 40;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 5;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 15;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 30;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 50;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 25;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 35;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 36;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 34;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 33;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 32;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 30;
    stub_set_remove(a, &tmp, __LINE__);
    tmp = 32;
    assert(stub_set_contains(a, &tmp, __LINE__));
    stub_set_clear(a, __LINE__);
    // One sided tree.
    tmp = 10;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 9;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 8;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 7;
    stub_set_add(a, &tmp, __LINE__);
    tmp = 8;
    stub_set_remove(a, &tmp, __LINE__);
    tmp = 7;
    assert(stub_set_contains(a, &tmp, __LINE__));
    stub_set_destroy(a, __LINE__);
    // Replace two sided two children
    a = set_init(sizeof(int), compare_int);
    b = 5;
    stub_set_add(a, &b, __LINE__);
    b = 1;
    stub_set_add(a, &b, __LINE__);
    b = 6;
    stub_set_add(a, &b, __LINE__);
    b = -1;
    stub_set_add(a, &b, __LINE__);
    b = 3;
    stub_set_add(a, &b, __LINE__);
    b = 7;
    stub_set_add(a, &b, __LINE__);
    b = -2;
    stub_set_add(a, &b, __LINE__);
    b = 0;
    stub_set_add(a, &b, __LINE__);
    b = 2;
    stub_set_add(a, &b, __LINE__);
    b = 4;
    stub_set_add(a, &b, __LINE__);
    b = 1;
    stub_set_remove(a, &b, __LINE__);
    assert(!stub_set_contains(a, &b, __LINE__));
    stub_set_clear(a, __LINE__);
    b = 5;
    stub_set_add(a, &b, __LINE__);
    b = 1;
    stub_set_add(a, &b, __LINE__);
    b = 6;
    stub_set_add(a, &b, __LINE__);
    b = -1;
    stub_set_add(a, &b, __LINE__);
    b = 3;
    stub_set_add(a, &b, __LINE__);
    b = 7;
    stub_set_add(a, &b, __LINE__);
    b = -2;
    stub_set_add(a, &b, __LINE__);
    b = 0;
    stub_set_add(a, &b, __LINE__);
    b = 4;
    stub_set_add(a, &b, __LINE__);
    b = 1;
    stub_set_remove(a, &b, __LINE__);
    assert(!stub_set_contains(a, &b, __LINE__));
    stub_set_destroy(a, __LINE__);
}
