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

static void mutation_order(set me, const int *const arr, const int size)
{
    int i;
    int actual_size = 0;
    assert(set_is_empty(me));
    for (i = 0; i < size; i++) {
        int num = arr[i];
        if (num > 0) {
            assert(set_put(me, &num) == 0);
            actual_size++;
        } else {
            int actual_num = -1 * num;
            assert(set_remove(me, &actual_num));
            actual_size--;
        }
    }
    assert(set_size(me) == actual_size);
    set_verify(me);
}

/*
 * Targets the (child->balance == 0) branch.
 */
static void test_rotate_left_balanced_child(set me)
{
    int i;
    int arr[] = {2, 4, 1, 3, 5, -1};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 2; i <= 5; i++) {
        assert(set_contains(me, &i));
    }
}

/*
 * Targets the else branch.
 */
static void test_rotate_left_unbalanced_child(set me)
{
    int i;
    int arr[] = {1, 2, 3};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 3; i++) {
        assert(set_contains(me, &i));
    }
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
    int i;
    int arr[] = {4, 2, 5, 1, 3, -5};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 4; i++) {
        assert(set_contains(me, &i));
    }
}

/*
 * Targets the else branch.
 */
static void test_rotate_right_unbalanced_child(set me)
{
    int i;
    int arr[] = {3, 2, 1};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 3; i++) {
        assert(set_contains(me, &i));
    }
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
    int i;
    int arr[] = {5, 2, 6, 1, 3, 4};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 6; i++) {
        assert(set_contains(me, &i));
    }
}

/*
 * Targets the (grand_child->balance == 0) branch.
 */
static void test_rotate_left_right_neutral_balanced_grand_child(set me)
{
    int i;
    int arr[] = {3, 1, 2};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 3; i++) {
        assert(set_contains(me, &i));
    }
}

/*
 * Targets the else branch.
 */
static void test_rotate_left_right_negatively_balanced_grand_child(set me)
{
    int i;
    int arr[] = {5, 2, 6, 1, 4, 3};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 6; i++) {
        assert(set_contains(me, &i));
    }
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
    int i;
    int arr[] = {2, 1, 5, 3, 6, 4};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 6; i++) {
        assert(set_contains(me, &i));
    }
}

/*
 * Targets the (grand_child->balance == 0) branch.
 */
static void test_rotate_right_left_neutral_balanced_grand_child(set me)
{
    int i;
    int arr[] = {1, 3, 2};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 3; i++) {
        assert(set_contains(me, &i));
    }
}

/*
 * Targets the else branch.
 */
static void test_rotate_right_left_negatively_balanced_grand_child(set me)
{
    int i;
    int arr[] = {2, 1, 5, 4, 6, 3};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 6; i++) {
        assert(set_contains(me, &i));
    }
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

static void test_put_already_existing(void)
{
    int key = 5;
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    assert(set_size(me) == 0);
    set_put(me, &key);
    assert(set_size(me) == 1);
    set_put(me, &key);
    assert(set_size(me) == 1);
    assert(!set_destroy(me));
}

static void test_remove_nothing(void)
{
    int key;
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    key = 3;
    set_put(me, &key);
    key = 5;
    assert(!set_remove(me, &key));
    assert(!set_destroy(me));
}

static void test_contains(void)
{
    int key;
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    key = 7;
    assert(!set_contains(me, &key));
    key = 3;
    set_put(me, &key);
    key = 1;
    set_put(me, &key);
    key = 5;
    set_put(me, &key);
    key = 0;
    assert(!set_contains(me, &key));
    key = 1;
    assert(set_contains(me, &key));
    key = 2;
    assert(!set_contains(me, &key));
    key = 3;
    assert(set_contains(me, &key));
    key = 4;
    assert(!set_contains(me, &key));
    key = 5;
    assert(set_contains(me, &key));
    key = 6;
    assert(!set_contains(me, &key));
    assert(!set_destroy(me));
}

static void test_stress_add(void)
{
    int count = 0;
    int flip = 0;
    int i;
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    for (i = 1234; i < 82400; i++) {
        int is_already_present;
        int num = i % 765;
        is_already_present = set_contains(me, &num);
        set_put(me, &num);
        assert(set_contains(me, &num));
        if (!is_already_present) {
            count++;
        }
        if (i == 1857 && !flip) {
            i *= -1;
            flip = 1;
        }
    }
    assert(count == set_size(me));
    assert(!set_destroy(me));
}

static void test_stress_remove(void)
{
    int i;
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    for (i = 8123; i < 12314; i += 3) {
        set_put(me, &i);
        assert(set_contains(me, &i));
    }
    for (i = 13000; i > 8000; i--) {
        set_remove(me, &i);
        assert(!set_contains(me, &i));
    }
    assert(!set_destroy(me));
}

static void test_unique_delete_one_child(set me)
{
    int arr1[] = {2, 1, -2};
    int arr2[] = {1, 2, -1};
    int arr3[] = {3, 2, 4, 1, -2};
    int arr4[] = {3, 1, 4, 2, -1};
    int arr5[] = {3, 1, 4, 2, -4};
    int arr6[] = {2, 1, 3, 4, -3};
    int sz1 = sizeof(arr1) / sizeof(arr1[0]);
    int sz2 = sizeof(arr2) / sizeof(arr2[0]);
    int sz3 = sizeof(arr3) / sizeof(arr3[0]);
    int sz4 = sizeof(arr4) / sizeof(arr4[0]);
    int sz5 = sizeof(arr5) / sizeof(arr5[0]);
    int sz6 = sizeof(arr6) / sizeof(arr6[0]);
    mutation_order(me, arr1, sz1);
    set_clear(me);
    mutation_order(me, arr2, sz2);
    set_clear(me);
    mutation_order(me, arr3, sz3);
    set_clear(me);
    mutation_order(me, arr4, sz4);
    set_clear(me);
    mutation_order(me, arr5, sz5);
    set_clear(me);
    mutation_order(me, arr6, sz6);
}

static void test_unique_delete_two_children(set me)
{
    int arr1[] = {2, 1, 3, -2};
    int arr2[] = {4, 2, 5, 1, 3, -2};
    int arr3[] = {2, 1, 4, 3, 5, -4};
    int sz1 = sizeof(arr1) / sizeof(arr1[0]);
    int sz2 = sizeof(arr2) / sizeof(arr2[0]);
    int sz3 = sizeof(arr3) / sizeof(arr3[0]);
    mutation_order(me, arr1, sz1);
    set_clear(me);
    mutation_order(me, arr2, sz2);
    set_clear(me);
    mutation_order(me, arr3, sz3);
}

static void test_unique_deletion_patterns(void)
{
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    test_unique_delete_one_child(me);
    set_clear(me);
    test_unique_delete_two_children(me);
    assert(!set_destroy(me));
}

static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!set_init(sizeof(int), compare_int));
}

static void test_put_root_out_of_memory(set me)
{
    int key = 2;
    fail_malloc = 1;
    assert(set_put(me, &key) == -ENOMEM);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(set_put(me, &key) == -ENOMEM);
}

static void test_put_on_left_out_of_memory(set me)
{
    int key = 1;
    fail_malloc = 1;
    assert(set_put(me, &key) == -ENOMEM);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(set_put(me, &key) == -ENOMEM);
}

static void test_put_on_right_out_of_memory(set me)
{
    int key = 3;
    fail_malloc = 1;
    assert(set_put(me, &key) == -ENOMEM);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(set_put(me, &key) == -ENOMEM);
}

static void test_put_out_of_memory(void)
{
    int key = 2;
    set me = set_init(sizeof(int), compare_int);
    assert(me);
    test_put_root_out_of_memory(me);
    assert(set_put(me, &key) == 0);
    test_put_on_left_out_of_memory(me);
    test_put_on_right_out_of_memory(me);
    assert(!set_destroy(me));
}

void test_set(void)
{
    test_invalid_init();
    test_auto_balancing();
    test_put_already_existing();
    test_remove_nothing();
    test_contains();
    test_stress_add();
    test_stress_remove();
    test_unique_deletion_patterns();
    test_init_out_of_memory();
    test_put_out_of_memory();
}
