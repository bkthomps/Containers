#include "test.h"
#include "../src/list.h"

void test_list(void)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    list me = list_init(sizeof(int));
    assert(me);
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
    list_copy_to_array(get_arr, me);
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
    list_copy_to_array(trimmed, me);
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
    int get = 0xdeadbeef;
    list_get_first(&get, me);
    assert(get == 10);
    get = 0xdeadbeef;
    list_get_at(&get, me, 0);
    assert(get == 10);
    list_get_at(&get, me, 1);
    assert(get == -1);
    list_get_at(&get, me, 2);
    assert(get == 9);
    list_get_at(&get, me, 3);
    assert(get == 8);
    list_get_at(&get, me, 4);
    assert(get == 3);
    list_get_at(&get, me, 5);
    assert(get == -2);
    get = 0xdeadbeef;
    list_get_last(&get, me);
    assert(get == -2);
    list_remove_first(me);
    list_remove_at(me, 2);
    list_remove_last(me);
    assert(list_size(me) == 3);
    get = 345;
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
    list_copy_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    list_set_at(me, 0, &set);
    set = -6;
    list_set_at(me, 1, &set);
    set = -7;
    list_set_at(me, 2, &set);
    list_copy_to_array(arr, me);
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
    assert(!me);
}
