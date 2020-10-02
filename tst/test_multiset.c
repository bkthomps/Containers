#include "test.h"
#include "../src/include/multiset.h"

/*
 * Include this to verify the tree.
 */
struct internal_multiset {
    size_t size;
    size_t key_size;
    int (*comparator)(const void *const one, const void *const two);
    char *root;
};

/*
 * Include this to verify the tree.
 */
static const size_t ptr_size = sizeof(char *);
/* static const size_t count_size = sizeof(size_t); */
/* Node balance is always the first byte (at index 0). */
/* static const size_t node_count_offset = sizeof(signed char); */
static const size_t node_parent_offset = 1 + sizeof(size_t);
static const size_t node_left_child_offset =
        1 + sizeof(size_t) + sizeof(char *);
static const size_t node_right_child_offset =
        1 + sizeof(size_t) + 2 * sizeof(char *);
static const size_t node_key_offset = 1 + sizeof(size_t) + 3 * sizeof(char *);

/*
 * Verifies that the AVL tree rules are followed. The balance factor of an item
 * must be the right height minus the left height. Also, the left key must be
 * less than the right key.
 */
static int multiset_verify_recursive(char *const item)
{
    int left;
    int right;
    int max;
    char *item_left;
    char *item_right;
    if (!item) {
        return 0;
    }
    memcpy(&item_left, item + node_left_child_offset, ptr_size);
    memcpy(&item_right, item + node_right_child_offset, ptr_size);
    left = multiset_verify_recursive(item_left);
    right = multiset_verify_recursive(item_right);
    max = left > right ? left : right;
    assert(right - left == item[0]);
    if (item_left && item_right) {
        const int left_val = *(int *) (item_left + node_key_offset);
        const int right_val = *(int *) (item_right + node_key_offset);
        assert(left_val < right_val);
    }
    if (item_left) {
        char *item_left_parent;
        memcpy(&item_left_parent, item_left + node_parent_offset, ptr_size);
        assert(item_left_parent == item);
        assert(item_left_parent + node_key_offset == item + node_key_offset);
    }
    if (item_right) {
        char *item_right_parent;
        memcpy(&item_right_parent, item_right + node_parent_offset, ptr_size);
        assert(item_right_parent == item);
        assert(item_right_parent + node_key_offset == item + node_key_offset);
    }
    return max + 1;
}

static size_t multiset_compute_size(char *const item)
{
    char *left;
    char *right;
    if (!item) {
        return 0;
    }
    memcpy(&left, item + node_left_child_offset, ptr_size);
    memcpy(&right, item + node_right_child_offset, ptr_size);
    return 1 + multiset_compute_size(left) + multiset_compute_size(right);
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
    const size_t max_size = -1;
    assert(!multiset_init(0, compare_int));
    assert(!multiset_init(sizeof(int), NULL));
    assert(!multiset_init(max_size, compare_int));
}

static void mutation_order(multiset me, const int *const arr, const int size)
{
    int i;
    size_t actual_size = 0;
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
    size_t count = 0;
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
    assert(multiset_size(me) == 3);
    assert(multiset_remove(me, &key));
    assert(multiset_count(me, &key) == 1);
    assert(multiset_size(me) == 2);
    assert(multiset_remove(me, &key));
    assert(multiset_count(me, &key) == 0);
    assert(multiset_size(me) == 1);
    multiset_remove_all(me, &key);
    assert(multiset_size(me) == 1);
    key = 7;
    multiset_remove_all(me, &key);
    assert(multiset_size(me) == 0);
    assert(!multiset_destroy(me));
}

#if STUB_MALLOC
static void test_init_out_of_memory(void)
{
    fail_malloc = 1;
    assert(!multiset_init(sizeof(int), compare_int));
}
#endif

#if STUB_MALLOC
static void test_put_root_out_of_memory(multiset me)
{
    int key = 2;
    fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
}
#endif

#if STUB_MALLOC
static void test_put_on_left_out_of_memory(multiset me)
{
    int key = 1;
    fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
}
#endif

#if STUB_MALLOC
static void test_put_on_right_out_of_memory(multiset me)
{
    int key = 3;
    fail_malloc = 1;
    assert(multiset_put(me, &key) == -ENOMEM);
}
#endif

#if STUB_MALLOC
static void test_put_out_of_memory(void)
{
    int key = 2;
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(me);
    test_put_root_out_of_memory(me);
    assert(multiset_put(me, &key) == 0);
    test_put_on_left_out_of_memory(me);
    test_put_on_right_out_of_memory(me);
    assert(!multiset_destroy(me));
}
#endif

struct big_object {
    int n;
    double d;
    signed char c[8];
};

static int compare_big_object(const void *const one, const void *const two)
{
    const struct big_object *const a = one;
    const struct big_object *const b = two;
    return a->n - b->n;
}

static void test_big_object(void)
{
    int i;
    multiset me = multiset_init(sizeof(struct big_object), compare_big_object);
    assert(me);
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        b.n = INT_MIN + i;
        b.d = i + 0.5;
        for (j = 0; j < 8; j++) {
            b.c[j] = (signed char) (SCHAR_MIN + i + j);
        }
        assert(multiset_put(me, &b) == 0);
        b.n = -1;
        b.d = -1;
        for (j = 0; j < 8; j++) {
            b.c[j] = -1;
        }
    }
    for (i = 0; i < 16; i++) {
        int j;
        struct big_object b;
        b.n = INT_MIN + i;
        b.d = i + 0.5;
        for (j = 0; j < 8; j++) {
            b.c[j] = (signed char) (SCHAR_MIN + i + j);
        }
        assert(multiset_contains(me, &b) == 1);
        assert(b.n == INT_MIN + i);
        assert(b.d == i + 0.5);
        for (j = 0; j < 8; j++) {
            assert(b.c[j] == SCHAR_MIN + i + j);
        }
    }
    assert(!multiset_destroy(me));
}

static void test_ordered_retrieval(void)
{
    int *get;
    int val = 4;
    multiset me = multiset_init(sizeof(int), compare_int);
    assert(!multiset_first(me));
    assert(!multiset_last(me));
    assert(multiset_put(me, &val) == BK_OK);
    get = multiset_first(me);
    assert(val == *get);
    get = multiset_last(me);
    assert(val == *get);
    val = 5;
    assert(multiset_put(me, &val) == BK_OK);
    val = 3;
    assert(multiset_put(me, &val) == BK_OK);
    get = multiset_first(me);
    assert(3 == *get);
    get = multiset_last(me);
    assert(5 == *get);
    /* Lower tests */
    val = 7;
    get = multiset_lower(me, &val);
    assert(5 == *get);
    val = 6;
    get = multiset_lower(me, &val);
    assert(5 == *get);
    val = 5;
    get = multiset_lower(me, &val);
    assert(4 == *get);
    val = 4;
    get = multiset_lower(me, &val);
    assert(3 == *get);
    val = 3;
    get = multiset_lower(me, &val);
    assert(!get);
    /* Higher tests */
    val = 1;
    get = multiset_higher(me, &val);
    assert(3 == *get);
    val = 2;
    get = multiset_higher(me, &val);
    assert(3 == *get);
    val = 3;
    get = multiset_higher(me, &val);
    assert(4 == *get);
    val = 4;
    get = multiset_higher(me, &val);
    assert(5 == *get);
    val = 5;
    get = multiset_higher(me, &val);
    assert(!get);
    /* Floor tests */
    val = 7;
    get = multiset_floor(me, &val);
    assert(5 == *get);
    val = 6;
    get = multiset_floor(me, &val);
    assert(5 == *get);
    val = 5;
    get = multiset_floor(me, &val);
    assert(5 == *get);
    val = 4;
    get = multiset_floor(me, &val);
    assert(4 == *get);
    val = 3;
    get = multiset_floor(me, &val);
    assert(3 == *get);
    val = 2;
    get = multiset_floor(me, &val);
    assert(!get);
    /* Ceiling tests */
    val = 1;
    get = multiset_ceiling(me, &val);
    assert(3 == *get);
    val = 2;
    get = multiset_ceiling(me, &val);
    assert(3 == *get);
    val = 3;
    get = multiset_ceiling(me, &val);
    assert(3 == *get);
    val = 4;
    get = multiset_ceiling(me, &val);
    assert(4 == *get);
    val = 5;
    get = multiset_ceiling(me, &val);
    assert(5 == *get);
    val = 6;
    get = multiset_ceiling(me, &val);
    assert(!get);
    multiset_destroy(me);
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
#if STUB_MALLOC
    test_init_out_of_memory();
    test_put_out_of_memory();
#endif
    test_big_object();
    test_ordered_retrieval();
    multiset_destroy(NULL);
}
