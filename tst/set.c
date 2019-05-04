#include "test.h"
#include "../src/set.h"

/*
 * Include this struct to verify the tree.
 */
struct internal_set {
    size_t key_size;
    int (*comparator)(const void *const one, const void *const two);
    int size;
    struct node *root;
};

/*
 * Include this struct to verify the tree.
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
    int left;
    int right;
    int max;
    if (!item) {
        return 0;
    }
    left = set_verify_recursive(item->left);
    right = set_verify_recursive(item->right);
    max = left > right ? left : right;
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
    set_verify_recursive(me->root);
    assert(set_compute_size(me->root) == set_size(me));
}

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

static void test_invalid_init(void)
{
    assert(!set_init(0, compare_int));
    assert(!set_init(sizeof(int), NULL));
}

/*
 * Targets the (child->balance == 0) branch.
 */
static void test_rotate_left_balanced_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 2;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 1;
    set_remove(me, &key);
    assert(set_size(me) == 4);
    for (key = 2; key <= 5; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets the else branch.
 */
static void test_rotate_left_unbalanced_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 1;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    assert(set_size(me) == 3);
    for (key = 1; key <= 3; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets (parent->balance == 2 && child->balance >= 0) in the set_repair
 * function.
 */
static void test_rotate_left(void)
{
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    test_rotate_left_balanced_child(me);
    set_clear(me);
    test_rotate_left_unbalanced_child(me);
    assert(!set_destroy(me));
}

/*
 * Targets the (child->balance == 0) branch.
 */
static void test_rotate_right_balanced_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 4;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 5;
    set_remove(me, &key);
    assert(set_size(me) == 4);
    for (key = 1; key <= 4; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets the else branch.
 */
static void test_rotate_right_unbalanced_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 3;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    assert(set_size(me) == 3);
    for (key = 1; key <= 3; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets (parent->balance == -2 && child->balance <= 0) in the set_repair
 * function.
 */
static void test_rotate_right(void)
{
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    test_rotate_right_balanced_child(me);
    set_clear(me);
    test_rotate_right_unbalanced_child(me);
    assert(!set_destroy(me));
}

/*
 * Targets the (grand_child->balance == 1) branch.
 */
static void test_rotate_left_right_positively_balanced_grand_child(set me)
{
    // TODO
}

/*
 * Targets the (grand_child->balance == 0) branch.
 */
static void test_rotate_left_right_neutral_balanced_grand_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 3;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    assert(set_size(me) == 3);
    for (key = 1; key <= 3; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets the else branch.
 */
static void test_rotate_left_right_negatively_balanced_grand_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 5;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    key = 6;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    assert(set_size(me) == 6);
    for (key = 1; key <= 6; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets (parent->balance == -2 && child->balance == 1) in the set_repair
 * function.
 */
static void test_rotate_left_right(void)
{
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    test_rotate_left_right_positively_balanced_grand_child(me);
    set_clear(me);
    test_rotate_left_right_neutral_balanced_grand_child(me);
    set_clear(me);
    test_rotate_left_right_negatively_balanced_grand_child(me);
    assert(!set_destroy(me));
}

/*
 * Targets the (grand_child->balance == 1) branch.
 */
static void test_rotate_right_left_positively_balanced_grand_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 2;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 6;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    assert(set_size(me) == 6);
    for (key = 1; key <= 6; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets the (grand_child->balance == 0) branch.
 */
static void test_rotate_right_left_neutral_balanced_grand_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 1;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    key = 2;
    set_put(me, &key);
    assert(set_size(me) == 3);
    for (key = 1; key <= 3; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets the else branch.
 */
static void test_rotate_right_left_negatively_balanced_grand_child(set me)
{
    int key;
    assert(set_is_empty(me));
    key = 2;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 4;
    set_put(me, &key);
    key = 6;
    set_put(me, &key);
    key = 3;
    set_put(me, &key);
    assert(set_size(me) == 6);
    for (key = 1; key <= 6; key++) {
        assert(set_contains(me, &key));
    }
    set_verify(me);
}

/*
 * Targets (parent->balance == 2 && child->balance == -1) in the set_repair
 * function.
 */
static void test_rotate_right_left(void)
{
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    test_rotate_right_left_positively_balanced_grand_child(me);
    set_clear(me);
    test_rotate_right_left_neutral_balanced_grand_child(me);
    set_clear(me);
    test_rotate_right_left_negatively_balanced_grand_child(me);
    assert(!set_destroy(me));
}

/*
 * Targets the set_repair function.
 */
static void test_auto_balancing(void)
{
    test_rotate_left();
    test_rotate_right();
    test_rotate_left_right();
    test_rotate_right_left();
}

static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!set_init(sizeof(int), compare_int));
}

void test_set(void)
{
    test_invalid_init();
    test_auto_balancing();
    test_init_out_of_memory();
}
