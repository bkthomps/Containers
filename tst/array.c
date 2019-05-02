#include "test.h"
#include "../src/array.h"

void test_array(void)
{
    array me;
    int i;
    int *data;
    int arr[10] = {0};
    int array[2] = {0xdeadbeef, 0xdeadbeef};
    int get;
    assert(!array_init(-1, sizeof(int)));
    assert(!array_init(1, 0));
    me = array_init(10, sizeof(int));
    assert(me);
    assert(array_size(me) == 10);
    for (i = 0; i < 10; i++) {
        get = 0xdeadbeef;
        array_get(&get, me, i);
        assert(get == 0);
    }
    for (i = 0; i < 10; i++) {
        get = 0xdeadbeef;
        array_set(me, i, &i);
        array_get(&get, me, i);
        assert(get == i);
    }
    for (i = 0; i < 10; i++) {
        get = 0xdeadbeef;
        array_get(&get, me, i);
        assert(get == i);
    }
    array_copy_to_array(arr, me);
    for (i = 0; i < 10; i++) {
        assert(arr[i] == i);
    }
    data = array_get_data(me);
    for (i = 0; i < 10; i++) {
        assert(data[i] == i);
    }
    get = 0xdeadbeef;
    assert(array_set(me, -1, &get) == -EINVAL);
    assert(array_get(&get, me, -1) == -EINVAL);
    me = array_destroy(me);
    assert(!me);
    me = array_init(0, sizeof(int));
    assert(array_size(me) == 0);
    array_copy_to_array(array, me);
    assert(array[0] == 0xdeadbeef);
    assert(array[1] == 0xdeadbeef);
    assert(!array_get_data(me));
    assert(array_set(me, 0, &get) == -EINVAL);
    assert(array_get(&get, me, 0) == -EINVAL);
    assert(!array_destroy(me));
}
