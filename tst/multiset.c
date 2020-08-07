#include "test.h"
#include "../src/include/multiset.h"

/*
 * Include this struct to verify the tree.
 */
struct internal_multiset {
    size_t key_size;
    int (*comparator)(const void *const one, const void *const two);
    int size;
    struct node *root;
};

/*
 * Include this struct to verify the tree.
 */
struct node {
    int count;
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
static int multiset_verify_recursive(struct node *const item)
{
    int left;
    int right;
    int max;
    if (!item) {
        return 0;
    }
    left = multiset_verify_recursive(item->left);
    right = multiset_verify_recursive(item->right);
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

static int multiset_compute_size(struct node *const item)
{
    if (!item) {
        return 0;
    }
    return 1 + multiset_compute_size(item->left) +
           multiset_compute_size(item->right);
}

static void multiset_verify(multiset me)
{
    multiset_verify_recursive(me->root);
    assert(multiset_compute_size(me->root) == multiset_size(me));
}

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

static void test_invalid_init(void)
{
    assert(!multiset_init(0, compare_int));
    assert(!multiset_init(sizeof(int), NULL));
}

static void mutation_order(multiset me, const int *const arr, const int size)
{
    int i;
    int actual_size = 0;
    assert(multiset_is_empty(me));
    for (i = 0; i < size; i++) {
        int num = arr[i];
        if (num > 0) {
            assert(multiset_put(me, &num) == 0);
            actual_size++;
        } else {
            int actual_num = -1 * num;
            assert(multiset_remove(me, &actual_num));
            actual_size--;
        }
    }
    assert(multiset_size(me) == actual_size);
    multiset_verify(me);
}

/*
 * Targets the (child->balance == 0) branch.
 */
static void test_rotate_left_balanced_child(multiset me)
{
    int i;
    int arr[] = {2, 4, 1, 3, 5, -1};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 2; i <= 5; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets the else branch.
 */
static void test_rotate_left_unbalanced_child(multiset me)
{
    int i;
    int arr[] = {1, 2, 3};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 3; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets (parent->balance == 2 && child->balance >= 0) in the multiset_repair
 * function.
 */
static void test_rotate_left(void)
{
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    test_rotate_left_balanced_child(me);
    multiset_clear(me);
    test_rotate_left_unbalanced_child(me);
    assert(!multiset_destroy(me));
}

/*
 * Targets the (child->balance == 0) branch.
 */
static void test_rotate_right_balanced_child(multiset me)
{
    int i;
    int arr[] = {4, 2, 5, 1, 3, -5};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 4; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets the else branch.
 */
static void test_rotate_right_unbalanced_child(multiset me)
{
    int i;
    int arr[] = {3, 2, 1};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 3; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets (parent->balance == -2 && child->balance <= 0) in the multiset_repair
 * function.
 */
static void test_rotate_right(void)
{
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    test_rotate_right_balanced_child(me);
    multiset_clear(me);
    test_rotate_right_unbalanced_child(me);
    assert(!multiset_destroy(me));
}

/*
 * Targets the (grand_child->balance == 1) branch.
 */
static void test_rotate_left_right_positively_balanced_grand_child(multiset me)
{
    int i;
    int arr[] = {5, 2, 6, 1, 3, 4};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 6; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets the (grand_child->balance == 0) branch.
 */
static void test_rotate_left_right_neutral_balanced_grand_child(multiset me)
{
    int i;
    int arr[] = {3, 1, 2};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 3; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets the else branch.
 */
static void test_rotate_left_right_negatively_balanced_grand_child(multiset me)
{
    int i;
    int arr[] = {5, 2, 6, 1, 4, 3};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 6; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets (parent->balance == -2 && child->balance == 1) in the multiset_repair
 * function.
 */
static void test_rotate_left_right(void)
{
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    test_rotate_left_right_positively_balanced_grand_child(me);
    multiset_clear(me);
    test_rotate_left_right_neutral_balanced_grand_child(me);
    multiset_clear(me);
    test_rotate_left_right_negatively_balanced_grand_child(me);
    assert(!multiset_destroy(me));
}

/*
 * Targets the (grand_child->balance == 1) branch.
 */
static void test_rotate_right_left_positively_balanced_grand_child(multiset me)
{
    int i;
    int arr[] = {2, 1, 5, 3, 6, 4};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 6; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets the (grand_child->balance == 0) branch.
 */
static void test_rotate_right_left_neutral_balanced_grand_child(multiset me)
{
    int i;
    int arr[] = {1, 3, 2};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 3; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets the else branch.
 */
static void test_rotate_right_left_negatively_balanced_grand_child(multiset me)
{
    int i;
    int arr[] = {2, 1, 5, 4, 6, 3};
    int size = sizeof(arr) / sizeof(arr[0]);
    mutation_order(me, arr, size);
    for (i = 1; i <= 6; i++) {
        assert(multiset_contains(me, &i));
    }
}

/*
 * Targets (parent->balance == 2 && child->balance == -1) in the multiset_repair
 * function.
 */
static void test_rotate_right_left(void)
{
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    test_rotate_right_left_positively_balanced_grand_child(me);
    multiset_clear(me);
    test_rotate_right_left_neutral_balanced_grand_child(me);
    multiset_clear(me);
    test_rotate_right_left_negatively_balanced_grand_child(me);
    assert(!multiset_destroy(me));
}

/*
 * Targets the multiset_repair function.
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
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    assert(multiset_size(me) == 0);
    multiset_put(me, &key);
    assert(multiset_size(me) == 1);
    multiset_put(me, &key);
    assert(multiset_size(me) == 2);
    assert(!multiset_destroy(me));
}

static void test_remove_nothing(void)
{
    int key;
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    key = 3;
    multiset_put(me, &key);
    key = 5;
    assert(!multiset_remove(me, &key));
    assert(!multiset_destroy(me));
}

static void test_contains(void)
{
    int key;
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    key = 7;
    assert(!multiset_contains(me, &key));
    key = 3;
    multiset_put(me, &key);
    key = 1;
    multiset_put(me, &key);
    key = 5;
    multiset_put(me, &key);
    key = 0;
    assert(!multiset_contains(me, &key));
    key = 1;
    assert(multiset_contains(me, &key));
    key = 2;
    assert(!multiset_contains(me, &key));
    key = 3;
    assert(multiset_contains(me, &key));
    key = 4;
    assert(!multiset_contains(me, &key));
    key = 5;
    assert(multiset_contains(me, &key));
    key = 6;
    assert(!multiset_contains(me, &key));
    assert(!multiset_destroy(me));
}

static void test_stress_add(void)
{
    int count = 0;
    int flip = 0;
    int i;
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    for (i = 1234; i < 82400; i++) {
        int num = i % 765;
        multiset_put(me, &num);
        assert(multiset_contains(me, &num));
        count++;
        if (i == 1857 && !flip) {
            i *= -1;
            flip = 1;
        }
    }
    assert(count == multiset_size(me));
    assert(!multiset_destroy(me));
}

static void test_stress_remove(void)
{
    int i;
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    for (i = 8123; i < 12314; i += 3) {
        multiset_put(me, &i);
        assert(multiset_contains(me, &i));
    }
    for (i = 13000; i > 8000; i--) {
        multiset_remove(me, &i);
        assert(!multiset_contains(me, &i));
    }
    assert(!multiset_destroy(me));
}

static void test_unique_delete_one_child(multiset me)
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
    multiset_clear(me);
    mutation_order(me, arr2, sz2);
    multiset_clear(me);
    mutation_order(me, arr3, sz3);
    multiset_clear(me);
    mutation_order(me, arr4, sz4);
    multiset_clear(me);
    mutation_order(me, arr5, sz5);
    multiset_clear(me);
    mutation_order(me, arr6, sz6);
}

static void test_unique_delete_two_children(multiset me)
{
    int arr1[] = {2, 1, 3, -2};
    int arr2[] = {4, 2, 5, 1, 3, -2};
    int arr3[] = {2, 1, 4, 3, 5, -4};
    int sz1 = sizeof(arr1) / sizeof(arr1[0]);
    int sz2 = sizeof(arr2) / sizeof(arr2[0]);
    int sz3 = sizeof(arr3) / sizeof(arr3[0]);
    mutation_order(me, arr1, sz1);
    multiset_clear(me);
    mutation_order(me, arr2, sz2);
    multiset_clear(me);
    mutation_order(me, arr3, sz3);
}

static void test_unique_deletion_patterns(void)
{
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    test_unique_delete_one_child(me);
    multiset_clear(me);
    test_unique_delete_two_children(me);
    assert(!multiset_destroy(me));
}

static void test_multiple_operations(void)
{
    int key = 5;
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    assert(multiset_count(me, &key) == 0);
    assert(!multiset_remove_all(me, &key));
    multiset_put(me, &key);
    multiset_put(me, &key);
    assert(multiset_size(me) == 2);
    key = 7;
    multiset_put(me, &key);
    assert(multiset_size(me) == 3);
    assert(multiset_count(me, &key) == 1);
    key = 5;
    assert(multiset_count(me, &key) == 2);
    multiset_remove_all(me, &key);
    assert(multiset_size(me) == 1);
    key = 7;
    multiset_remove_all(me, &key);
    assert(multiset_size(me) == 0);
    assert(!multiset_destroy(me));
}

static void test_init_out_of_memory(void)
{
#if STUB_MALLOC
    fail_malloc = 1;
    assert(!multiset_init(sizeof(int), compare_int));
#endif
}

static void test_put_root_out_of_memory(multiset me)
{
#if STUB_MALLOC
    int key = 2;
    fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
#endif
}

static void test_put_on_left_out_of_memory(multiset me)
{
#if STUB_MALLOC
    int key = 1;
    fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
#endif
}

static void test_put_on_right_out_of_memory(multiset me)
{
#if STUB_MALLOC
    int key = 3;
    fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
    fail_malloc = 1;
    delay_fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
#endif
}

static void test_put_out_of_memory(void)
{
#if STUB_MALLOC
    int key = 2;
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    test_put_root_out_of_memory(me);
    assert(multiset_put(me, &key) == 0);
    test_put_on_left_out_of_memory(me);
    test_put_on_right_out_of_memory(me);
    assert(!multiset_destroy(me));
#endif
}

void test_multiset(void)
{
    test_invalid_init();
    test_auto_balancing();
    test_put_already_existing();
    test_remove_nothing();
    test_contains();
    test_stress_add();
    test_stress_remove();
    test_unique_deletion_patterns();
    test_multiple_operations();
    test_init_out_of_memory();
    test_put_out_of_memory();
}
