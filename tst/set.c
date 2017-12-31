#include "test.h"
#include "../src/set.h"

// Used for thorough testing, but takes longer to run.
#define LONG_TEST

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
static int set_verify_recursive(set me, struct node *const item)
{
    if (item == NULL) {
        return 0;
    }
    const int left = set_verify_recursive(me, item->left);
    const int right = set_verify_recursive(me, item->right);
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

static void set_verify(set me)
{
#ifdef LONG_TEST
    set_verify_recursive(me, me->root);
    assert(set_compute_size(me->root) == set_size(me));
#endif
}

static int stub_set_add(set me, void *const key)
{
    const int ret = set_add(me, key);
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
    set a = set_init(sizeof(int), compare_int);
    int b;
    // left-left
    b = 5;
    stub_set_add(a, &b);
    b = 3;
    stub_set_add(a, &b);
    b = 1;
    stub_set_add(a, &b);
    b = 0xdeadbeef;
    stub_set_contains(a, &b);
    stub_set_clear(a);
    // right-right
    b = 1;
    stub_set_add(a, &b);
    b = 3;
    stub_set_add(a, &b);
    b = 5;
    stub_set_add(a, &b);
    b = 0xdeadbeef;
    stub_set_contains(a, &b);
    stub_set_clear(a);
    // left-right
    b = 5;
    stub_set_add(a, &b);
    b = 1;
    stub_set_add(a, &b);
    b = 3;
    stub_set_add(a, &b);
    b = 0xdeadbeef;
    stub_set_contains(a, &b);
    stub_set_clear(a);
    // right-left
    b = 1;
    stub_set_add(a, &b);
    b = 5;
    stub_set_add(a, &b);
    b = 3;
    stub_set_add(a, &b);
    b = 0xdeadbeef;
    stub_set_contains(a, &b);
    stub_set_clear(a);
    // Two children edge case.
    b = 8;
    stub_set_add(a, &b);
    b = 5;
    stub_set_add(a, &b);
    b = 11;
    stub_set_add(a, &b);
    b = 2;
    stub_set_add(a, &b);
    b = 6;
    stub_set_add(a, &b);
    b = 10;
    stub_set_add(a, &b);
    b = 15;
    stub_set_add(a, &b);
    b = 1;
    stub_set_add(a, &b);
    b = 3;
    stub_set_add(a, &b);
    b = 4;
    stub_set_add(a, &b);
    b = 7;
    stub_set_add(a, &b);
    b = 9;
    stub_set_add(a, &b);
    b = 12;
    stub_set_add(a, &b);
    b = 13;
    stub_set_add(a, &b);
    b = 16;
    stub_set_add(a, &b);
    b = 14;
    stub_set_add(a, &b);
    stub_set_clear(a);
    // Two children edge case.
    b = 8;
    stub_set_add(a, &b);
    b = 4;
    stub_set_add(a, &b);
    b = 12;
    stub_set_add(a, &b);
    b = 2;
    stub_set_add(a, &b);
    b = 6;
    stub_set_add(a, &b);
    b = 10;
    stub_set_add(a, &b);
    b = 15;
    stub_set_add(a, &b);
    b = 1;
    stub_set_add(a, &b);
    b = 3;
    stub_set_add(a, &b);
    b = 5;
    stub_set_add(a, &b);
    b = 7;
    stub_set_add(a, &b);
    b = 9;
    stub_set_add(a, &b);
    b = 11;
    stub_set_add(a, &b);
    b = 13;
    stub_set_add(a, &b);
    b = 16;
    stub_set_add(a, &b);
    b = 14;
    stub_set_add(a, &b);
    stub_set_clear(a);
    // Add a lot of items.
    int count = 0;
    bool flip = false;
    for (int i = 1234; i < 82400; i++) {
        int n = i % 765;
        const bool is_already_present = stub_set_contains(a, &n);
        stub_set_add(a, &n);
        const bool is_now_present = stub_set_contains(a, &n);
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
    stub_set_contains(a, &b);
    stub_set_destroy(a);
    a = set_init(sizeof(int), compare_int);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    b = 4;
    stub_set_add(a, &b);
    assert(set_size(a) == 1);
    stub_set_add(a, &b);
    assert(set_size(a) == 1);
    assert(!set_is_empty(a));
    assert(stub_set_contains(a, &b));
    b = 7;
    assert(!stub_set_contains(a, &b));
    stub_set_add(a, &b);
    assert(set_size(a) == 2);
    assert(stub_set_contains(a, &b));
    int c[10] = {5, 9, 4, -5, 0, 6, 1, 5, 7, 2};
    for (int i = 0; i < 10; i++) {
        stub_set_add(a, &c[i]);
        assert(stub_set_contains(a, &c[i]));
    }
    assert(set_size(a) == 9);
    for (int i = 0; i < 10; i++) {
        assert(stub_set_contains(a, &c[i]));
    }
    for (int i = -100; i < 100; i++) {
        bool contains = false;
        for (int j = 0; j < 10; j++) {
            if (c[j] == i) {
                contains = true;
            }
        }
        assert(stub_set_contains(a, &i) == contains);
    }
    int num = -3;
    assert(!stub_set_remove(a, &num));
    assert(set_size(a) == 9);
    assert(!stub_set_contains(a, &num));
    num = 6;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 8);
    assert(!stub_set_contains(a, &num));
    num = 4;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 7);
    assert(!stub_set_contains(a, &num));
    num = 7;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 6);
    assert(!stub_set_contains(a, &num));
    num = 9;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 5);
    assert(!stub_set_contains(a, &num));
    num = -5;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 4);
    assert(!stub_set_contains(a, &num));
    num = 0;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 3);
    assert(!stub_set_contains(a, &num));
    num = 1;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 2);
    assert(!stub_set_contains(a, &num));
    num = 5;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 1);
    assert(!stub_set_contains(a, &num));
    num = 2;
    assert(stub_set_remove(a, &num));
    assert(set_size(a) == 0);
    assert(!stub_set_contains(a, &num));
    // Add a lot of items and remove individually.
    for (int i = 5000; i < 6000; i++) {
        stub_set_add(a, &i);
        assert(stub_set_contains(a, &i));
    }
    assert(set_size(a) == 1000);
    for (int i = 5000; i < 5500; i++) {
        stub_set_remove(a, &i);
        assert(!stub_set_contains(a, &i));
    }
    assert(set_size(a) == 500);
    assert(!set_is_empty(a));
    stub_set_clear(a);
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    // Add a lot of items and clear.
    for (int i = 5000; i < 6000; i++) {
        stub_set_add(a, &i);
        assert(stub_set_contains(a, &i));
    }
    assert(set_size(a) == 1000);
    stub_set_clear(a);
    int p = 0xdeadbeef;
    assert(!stub_set_remove(a, &p));
    assert(set_size(a) == 0);
    assert(set_is_empty(a));
    a = stub_set_destroy(a);
    assert(a == NULL);
    // Create odd shape graph.
    a = set_init(sizeof(int), compare_int);
    int tmp = 10;
    stub_set_add(a, &tmp);
    tmp = 5;
    stub_set_add(a, &tmp);
    tmp = 15;
    stub_set_add(a, &tmp);
    tmp = 3;
    stub_set_add(a, &tmp);
    tmp = 8;
    stub_set_add(a, &tmp);
    tmp = 12;
    stub_set_add(a, &tmp);
    tmp = 18;
    stub_set_add(a, &tmp);
    tmp = 12;
    stub_set_remove(a, &tmp);
    tmp = 5;
    stub_set_remove(a, &tmp);
    tmp = 3;
    stub_set_remove(a, &tmp);
    tmp = 8;
    stub_set_remove(a, &tmp);
    stub_set_clear(a);
    // Allocate many nodes.
    for (int i = 8123; i < 12314; i += 3) {
        stub_set_add(a, &i);
        assert(stub_set_contains(a, &i));
    }
    for (int i = 13000; i > 8000; i--) {
        stub_set_remove(a, &i);
        assert(!stub_set_contains(a, &i));
    }
    stub_set_clear(a);
    // Create another odd shape graph.
    tmp = 20;
    stub_set_add(a, &tmp);
    tmp = 10;
    stub_set_add(a, &tmp);
    tmp = 40;
    stub_set_add(a, &tmp);
    tmp = 5;
    stub_set_add(a, &tmp);
    tmp = 15;
    stub_set_add(a, &tmp);
    tmp = 30;
    stub_set_add(a, &tmp);
    tmp = 50;
    stub_set_add(a, &tmp);
    tmp = 25;
    stub_set_add(a, &tmp);
    tmp = 35;
    stub_set_add(a, &tmp);
    tmp = 36;
    stub_set_add(a, &tmp);
    tmp = 34;
    stub_set_add(a, &tmp);
    tmp = 33;
    stub_set_add(a, &tmp);
    tmp = 32;
    stub_set_add(a, &tmp);
    tmp = 30;
    stub_set_remove(a, &tmp);
    tmp = 32;
    assert(stub_set_contains(a, &tmp));
    stub_set_clear(a);
    // One sided tree.
    tmp = 10;
    stub_set_add(a, &tmp);
    tmp = 9;
    stub_set_add(a, &tmp);
    tmp = 8;
    stub_set_add(a, &tmp);
    tmp = 7;
    stub_set_add(a, &tmp);
    tmp = 8;
    stub_set_remove(a, &tmp);
    tmp = 7;
    assert(stub_set_contains(a, &tmp));
    stub_set_destroy(a);
    // Replace two sided two children.
    a = set_init(sizeof(int), compare_int);
    b = 5;
    stub_set_add(a, &b);
    b = 1;
    stub_set_add(a, &b);
    b = 6;
    stub_set_add(a, &b);
    b = -1;
    stub_set_add(a, &b);
    b = 3;
    stub_set_add(a, &b);
    b = 7;
    stub_set_add(a, &b);
    b = -2;
    stub_set_add(a, &b);
    b = 0;
    stub_set_add(a, &b);
    b = 2;
    stub_set_add(a, &b);
    b = 4;
    stub_set_add(a, &b);
    b = 1;
    stub_set_remove(a, &b);
    assert(!stub_set_contains(a, &b));
    stub_set_clear(a);
    b = 5;
    stub_set_add(a, &b);
    b = 1;
    stub_set_add(a, &b);
    b = 6;
    stub_set_add(a, &b);
    b = -1;
    stub_set_add(a, &b);
    b = 3;
    stub_set_add(a, &b);
    b = 7;
    stub_set_add(a, &b);
    b = -2;
    stub_set_add(a, &b);
    b = 0;
    stub_set_add(a, &b);
    b = 4;
    stub_set_add(a, &b);
    b = 1;
    stub_set_remove(a, &b);
    assert(!stub_set_contains(a, &b));
    stub_set_destroy(a);
}
