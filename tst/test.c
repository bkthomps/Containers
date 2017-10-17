#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "../src/vector.h"
#include "../src/list.h"

static void test_vector(void) {
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector me = vector_init(sizeof(int));
    assert(me != NULL);
    assert(vector_size(me) == 0);
    assert(vector_is_empty(me));
    assert(vector_ensure_capacity(me, 8));
    assert(!vector_ensure_capacity(me, 9));
    for (int i = 0; i < 10; i++) {
        vector_add_first(me, &val[i]);
        int get = 0;
        vector_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(vector_size(me) == 10);
    assert(!vector_is_empty(me));
    assert(vector_ensure_capacity(me, 12));
    assert(!vector_ensure_capacity(me, 13));
    int get_arr[10] = {0};
    vector_to_array(get_arr, me);
    for (int i = 0; i < 10; i++) {
        int get = 0;
        vector_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    int trimmed[5] = {0};
    vector_trim_to_size(me);
    vector_set_space(me, 3);
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

int main() {
    test_vector();
    test_list();
    return 0;
}
