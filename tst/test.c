#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "../src/vector.h"
#include "../src/list.h"
#include "../src/forward_list.h"
#include "../src/deque.h"
#include "../src/stack.h"

static void test_vector(void) {
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector me = vector_init(sizeof(int));
    assert(me != NULL);
    assert(vector_size(me) == 0);
    assert(vector_is_empty(me));
    for (int i = 0; i < 10; i++) {
        vector_add_first(me, &val[i]);
        int get = 0;
        vector_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(vector_size(me) == 10);
    assert(!vector_is_empty(me));
    int get_arr[10] = {0};
    vector_to_array(get_arr, me);
    for (int i = 0; i < 10; i++) {
        int get = 0;
        vector_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    int trimmed[5] = {0};
    vector_trim(me);
    vector_reserve(me, 3);
    for (int i = 0; i < 7; i++) {
        vector_remove_last(me);
    }
    vector_to_array(trimmed, me);
    assert(vector_size(me) == 3);
    for (int i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
    int add = 3;
    vector_add_last(me, &add);
    add = -1;
    vector_add_at(me, 1, &add);
    add = -2;
    vector_add_last(me, &add);
    assert(vector_size(me) == 6);
    int aa = 456;
    int a = 456;
    int b = 456;
    int c = 456;
    int d = 456;
    int e = 456;
    int f = 456;
    int ff = 456;
    vector_get_first(&aa, me);
    assert(aa == 10);
    vector_get_at(&a, me, 0);
    assert(a == 10);
    vector_get_at(&b, me, 1);
    assert(b == -1);
    vector_get_at(&c, me, 2);
    assert(c == 9);
    vector_get_at(&d, me, 3);
    assert(d == 8);
    vector_get_at(&e, me, 4);
    assert(e == 3);
    vector_get_at(&f, me, 5);
    assert(f == -2);
    vector_get_last(&ff, me);
    assert(ff == -2);
    vector_remove_first(me);
    vector_remove_at(me, 2);
    vector_remove_last(me);
    assert(vector_size(me) == 3);
    int get = 345;
    vector_get_first(&get, me);
    assert(get == -1);
    vector_get_at(&get, me, 1);
    assert(get == 9);
    vector_get_last(&get, me);
    assert(get == 3);
    int set = 12;
    vector_set_first(me, &set);
    set = 13;
    vector_set_at(me, 1, &set);
    set = 14;
    vector_set_last(me, &set);
    int arr[3] = {0};
    vector_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    vector_set_at(me, 0, &set);
    set = -6;
    vector_set_at(me, 1, &set);
    set = -7;
    vector_set_at(me, 2, &set);
    vector_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
    assert(vector_set_at(me, 4, &set) == -EINVAL);
    assert(vector_get_at(&set, me, 4) == -EINVAL);
    assert(vector_remove_at(me, 4) == -EINVAL);
    assert(vector_add_at(me, 5, &set) == -EINVAL);
    assert(vector_set_at(me, -1, &set) == -EINVAL);
    assert(vector_get_at(&set, me, -1) == -EINVAL);
    assert(vector_remove_at(me, -1) == -EINVAL);
    assert(vector_add_at(me, -1, &set) == -EINVAL);
    vector_clear(me);
    assert(vector_size(me) == 0);
    assert(vector_is_empty(me));
    assert(vector_remove_first(me) == -EINVAL);
    assert(vector_remove_last(me) == -EINVAL);
    me = vector_destroy(me);
    assert(me == NULL);
}

static void test_list(void) {
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    list me = list_init(sizeof(int));
    assert(me != NULL);
    assert(list_size(me) == 0);
    assert(list_is_empty(me));
    for (int i = 0; i < 10; i++) {
        list_add_first(me, &val[i]);
        assert(list_size(me) == i + 1);
        int get = 0;
        list_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(list_size(me) == 10);
    assert(!list_is_empty(me));
    int get_arr[10] = {0};
    list_to_array(get_arr, me);
    for (int i = 0; i < 10; i++) {
        int get = 0;
        list_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    for (int i = 0; i < 7; i++) {
        list_remove_last(me);
    }
    int trimmed[5] = {0};
    list_to_array(trimmed, me);
    assert(list_size(me) == 3);
    for (int i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
    int add = 3;
    list_add_last(me, &add);
    add = -1;
    list_add_at(me, 1, &add);
    add = -2;
    list_add_last(me, &add);
    assert(list_size(me) == 6);
    int aa = 456;
    int a = 456;
    int b = 456;
    int c = 456;
    int d = 456;
    int e = 456;
    int f = 456;
    int ff = 456;
    list_get_first(&aa, me);
    assert(aa == 10);
    list_get_at(&a, me, 0);
    assert(a == 10);
    list_get_at(&b, me, 1);
    assert(b == -1);
    list_get_at(&c, me, 2);
    assert(c == 9);
    list_get_at(&d, me, 3);
    assert(d == 8);
    list_get_at(&e, me, 4);
    assert(e == 3);
    list_get_at(&f, me, 5);
    assert(f == -2);
    list_get_last(&ff, me);
    assert(ff == -2);
    list_remove_first(me);
    list_remove_at(me, 2);
    list_remove_last(me);
    assert(list_size(me) == 3);
    int get = 345;
    list_get_first(&get, me);
    assert(get == -1);
    list_get_at(&get, me, 1);
    assert(get == 9);
    list_get_last(&get, me);
    assert(get == 3);
    int set = 12;
    list_set_first(me, &set);
    set = 13;
    list_set_at(me, 1, &set);
    set = 14;
    list_set_last(me, &set);
    int arr[3] = {0};
    list_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    list_set_at(me, 0, &set);
    set = -6;
    list_set_at(me, 1, &set);
    set = -7;
    list_set_at(me, 2, &set);
    list_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
    assert(list_set_at(me, 4, &set) == -EINVAL);
    assert(list_get_at(&set, me, 4) == -EINVAL);
    assert(list_remove_at(me, 4) == -EINVAL);
    assert(list_add_at(me, 5, &set) == -EINVAL);
    assert(list_set_at(me, -1, &set) == -EINVAL);
    assert(list_get_at(&set, me, -1) == -EINVAL);
    assert(list_remove_at(me, -1) == -EINVAL);
    assert(list_add_at(me, -1, &set) == -EINVAL);
    list_clear(me);
    assert(list_size(me) == 0);
    assert(list_is_empty(me));
    assert(list_remove_first(me) == -EINVAL);
    assert(list_remove_last(me) == -EINVAL);
    me = list_destroy(me);
    assert(me == NULL);
}

static void test_forward_list(void) {
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    forward_list me = forward_list_init(sizeof(int));
    assert(me != NULL);
    assert(forward_list_size(me) == 0);
    assert(forward_list_is_empty(me));
    for (int i = 0; i < 10; i++) {
        forward_list_add_first(me, &val[i]);
        assert(forward_list_size(me) == i + 1);
        int get = 0;
        forward_list_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(forward_list_size(me) == 10);
    assert(!forward_list_is_empty(me));
    int get_arr[10] = {0};
    forward_list_to_array(get_arr, me);
    for (int i = 0; i < 10; i++) {
        int get = 0;
        forward_list_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    for (int i = 0; i < 7; i++) {
        forward_list_remove_last(me);
    }
    int trimmed[5] = {0};
    forward_list_to_array(trimmed, me);
    assert(forward_list_size(me) == 3);
    for (int i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
    int add = 3;
    forward_list_add_last(me, &add);
    add = -1;
    forward_list_add_at(me, 1, &add);
    add = -2;
    forward_list_add_last(me, &add);
    assert(forward_list_size(me) == 6);
    int aa = 456;
    int a = 456;
    int b = 456;
    int c = 456;
    int d = 456;
    int e = 456;
    int f = 456;
    int ff = 456;
    forward_list_get_first(&aa, me);
    assert(aa == 10);
    forward_list_get_at(&a, me, 0);
    assert(a == 10);
    forward_list_get_at(&b, me, 1);
    assert(b == -1);
    forward_list_get_at(&c, me, 2);
    assert(c == 9);
    forward_list_get_at(&d, me, 3);
    assert(d == 8);
    forward_list_get_at(&e, me, 4);
    assert(e == 3);
    forward_list_get_at(&f, me, 5);
    assert(f == -2);
    forward_list_get_last(&ff, me);
    assert(ff == -2);
    forward_list_remove_first(me);
    forward_list_remove_at(me, 2);
    forward_list_remove_last(me);
    assert(forward_list_size(me) == 3);
    int get = 345;
    forward_list_get_first(&get, me);
    assert(get == -1);
    forward_list_get_at(&get, me, 1);
    assert(get == 9);
    forward_list_get_last(&get, me);
    assert(get == 3);
    int set = 12;
    forward_list_set_first(me, &set);
    set = 13;
    forward_list_set_at(me, 1, &set);
    set = 14;
    forward_list_set_last(me, &set);
    int arr[3] = {0};
    forward_list_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    forward_list_set_at(me, 0, &set);
    set = -6;
    forward_list_set_at(me, 1, &set);
    set = -7;
    forward_list_set_at(me, 2, &set);
    forward_list_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
    assert(forward_list_set_at(me, 4, &set) == -EINVAL);
    assert(forward_list_get_at(&set, me, 4) == -EINVAL);
    assert(forward_list_remove_at(me, 4) == -EINVAL);
    assert(forward_list_add_at(me, 5, &set) == -EINVAL);
    assert(forward_list_set_at(me, -1, &set) == -EINVAL);
    assert(forward_list_get_at(&set, me, -1) == -EINVAL);
    assert(forward_list_remove_at(me, -1) == -EINVAL);
    assert(forward_list_add_at(me, -1, &set) == -EINVAL);
    forward_list_clear(me);
    assert(forward_list_size(me) == 0);
    assert(forward_list_is_empty(me));
    assert(forward_list_remove_first(me) == -EINVAL);
    assert(forward_list_remove_last(me) == -EINVAL);
    me = forward_list_destroy(me);
    assert(me == NULL);
}

static void test_deque(void) {
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    deque me = deque_init(sizeof(int));
    assert(me != NULL);
    assert(deque_size(me) == 0);
    assert(deque_is_empty(me));
    for (int i = 0; i < 10; i++) {
        deque_push_front(me, &val[i]);
        int get = 0;
        deque_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(deque_size(me) == 10);
    assert(!deque_is_empty(me));
    int get_arr[10] = {0};
    deque_to_array(get_arr, me);
    for (int i = 0; i < 10; i++) {
        int get = 0;
        deque_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    int trimmed[5] = {0};
    int stuff = 5;
    deque_trim(me);
    for (int i = 0; i < 7; i++) {
        deque_pop_back(&stuff, me);
        assert(stuff == val[i]);
    }
    deque_to_array(trimmed, me);
    assert(deque_size(me) == 3);
    for (int i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
    int add = 3;
    deque_push_back(me, &add);
    add = -2;
    deque_push_back(me, &add);
    add = -1;
    deque_push_back(me, &add);
    assert(deque_size(me) == 6);
    int aa = 456;
    int a = 456;
    int b = 456;
    int c = 456;
    int d = 456;
    int e = 456;
    int f = 456;
    int ff = 456;
    deque_get_first(&aa, me);
    assert(aa == 10);
    deque_get_at(&a, me, 0);
    assert(a == 10);
    deque_get_at(&b, me, 1);
    assert(b == 9);
    deque_get_at(&c, me, 2);
    assert(c == 8);
    deque_get_at(&d, me, 3);
    assert(d == 3);
    deque_get_at(&e, me, 4);
    assert(e == -2);
    deque_get_at(&f, me, 5);
    assert(f == -1);
    deque_get_last(&ff, me);
    assert(ff == -1);
    deque_pop_front(&stuff, me);
    assert(stuff == 10);
    deque_pop_back(&stuff, me);
    assert(stuff == -1);
    deque_pop_back(&stuff, me);
    assert(stuff == -2);
    assert(deque_size(me) == 3);
    int get = 345;
    deque_get_first(&get, me);
    assert(get == 9);
    deque_get_at(&get, me, 1);
    assert(get == 8);
    deque_get_last(&get, me);
    assert(get == 3);
    int set = 12;
    deque_set_first(me, &set);
    set = 13;
    deque_set_at(me, 1, &set);
    set = 14;
    deque_set_last(me, &set);
    int arr[3] = {0};
    deque_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    deque_set_at(me, 0, &set);
    set = -6;
    deque_set_at(me, 1, &set);
    set = -7;
    deque_set_at(me, 2, &set);
    deque_to_array(arr, me);
    assert(arr[0] == -5);
    assert(arr[1] == -6);
    assert(arr[2] == -7);
    assert(deque_set_at(me, 4, &set) == -EINVAL);
    assert(deque_get_at(&set, me, 4) == -EINVAL);
    assert(deque_set_at(me, -1, &set) == -EINVAL);
    assert(deque_get_at(&set, me, -1) == -EINVAL);
    deque_clear(me);
    assert(deque_size(me) == 0);
    assert(deque_is_empty(me));
    me = deque_destroy(me);
    assert(me == NULL);
}

static void test_stack(void) {
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    stack me = stack_init(sizeof(int));
    assert(me != NULL);
    assert(stack_size(me) == 0);
    assert(stack_is_empty(me));
    for (int i = 0; i < 10; i++) {
        stack_push(me, &val[i]);
        int get = 0;
        stack_top(&get, me);
        assert(get == val[i]);
    }
    assert(stack_size(me) == 10);
    assert(!stack_is_empty(me));
    int get_arr[10] = {0};
    stack_to_array(get_arr, me);
    for (int i = 0; i < 10; i++) {
        assert(get_arr[i] == i + 1);
    }
    for (int i = 0; i < 9; i++) {
        int get = 0;
        stack_pop(&get, me);
        assert(get == 10 - i);
    }
    stack_trim(me);
    assert(stack_size(me) == 1);
    stack_clear(me);
    assert(stack_size(me) == 0);
    me = stack_destroy(me);
    assert(me == NULL);
}

int main() {
    test_vector();
    test_list();
    test_forward_list();
    test_deque();
    test_stack();
    return 0;
}
