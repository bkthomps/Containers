#include "test.h"
#include "../src/forward_list.h"

void test_forward_list(void)
{
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int get_arr[10] = {0};
    int trimmed[5] = {0};
    int arr[3] = {0};
    int add;
    forward_list me;
    int get;
    int set;
    int i;
    assert(!forward_list_init(0));
    me = forward_list_init(sizeof(int));
    assert(me);
    assert(forward_list_size(me) == 0);
    assert(forward_list_is_empty(me));
    for (i = 0; i < 10; i++) {
        forward_list_add_first(me, &val[i]);
        assert(forward_list_size(me) == i + 1);
        get = 0;
        forward_list_get_first(&get, me);
        assert(get == val[i]);
    }
    assert(forward_list_size(me) == 10);
    assert(!forward_list_is_empty(me));
    forward_list_copy_to_array(get_arr, me);
    for (i = 0; i < 10; i++) {
        get = 0;
        forward_list_get_at(&get, me, i);
        assert(get == val[9 - i]);
        assert(get_arr[i] == val[9 - i]);
    }
    for (i = 0; i < 7; i++) {
        forward_list_remove_last(me);
    }
    forward_list_copy_to_array(trimmed, me);
    assert(forward_list_size(me) == 3);
    for (i = 0; i < 3; i++) {
        assert(10 - i == trimmed[i]);
    }
    add = 3;
    forward_list_add_last(me, &add);
    add = -1;
    forward_list_add_at(me, 1, &add);
    add = -2;
    forward_list_add_last(me, &add);
    assert(forward_list_size(me) == 6);
    get = 0xdeadbeef;
    forward_list_get_first(&get, me);
    assert(get == 10);
    get = 0xdeadbeef;
    forward_list_get_at(&get, me, 0);
    assert(get == 10);
    forward_list_get_at(&get, me, 1);
    assert(get == -1);
    forward_list_get_at(&get, me, 2);
    assert(get == 9);
    forward_list_get_at(&get, me, 3);
    assert(get == 8);
    forward_list_get_at(&get, me, 4);
    assert(get == 3);
    forward_list_get_at(&get, me, 5);
    assert(get == -2);
    get = 0xdeadbeef;
    forward_list_get_last(&get, me);
    assert(get == -2);
    forward_list_remove_first(me);
    forward_list_remove_at(me, 2);
    forward_list_remove_last(me);
    assert(forward_list_size(me) == 3);
    get = 345;
    forward_list_get_first(&get, me);
    assert(get == -1);
    forward_list_get_at(&get, me, 1);
    assert(get == 9);
    forward_list_get_last(&get, me);
    assert(get == 3);
    set = 12;
    forward_list_set_first(me, &set);
    set = 13;
    forward_list_set_at(me, 1, &set);
    set = 14;
    forward_list_set_last(me, &set);
    forward_list_copy_to_array(arr, me);
    assert(arr[0] == 12);
    assert(arr[1] == 13);
    assert(arr[2] == 14);
    set = -5;
    forward_list_set_at(me, 0, &set);
    set = -6;
    forward_list_set_at(me, 1, &set);
    set = -7;
    forward_list_set_at(me, 2, &set);
    forward_list_copy_to_array(arr, me);
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
    assert(!me);
}
