#include "test.h"
#include "../src/array.h"

void test_array(void)
{
    assert(!array_init(-1, sizeof(int)));
    assert(!array_init(1, 0));
    array me = array_init(10, sizeof(int));
    assert(me);
    assert(array_size(me) == 10);
    int i;
    for (i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_get(&get, me, i);
        assert(get == 0);
    }
    for (i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_set(me, i, &i);
        array_get(&get, me, i);
        assert(get == i);
    }
    for (i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_get(&get, me, i);
        assert(get == i);
    }
    int arr[10] = {0};
    array_copy_to_array(arr, me);
    for (i = 0; i < 10; i++) {
        assert(arr[i] == i);
    }
    int *const data = array_get_data(me);
    for (i = 0; i < 10; i++) {
        assert(data[i] == i);
    }
    int get = 0xdeadbeef;
    assert(array_set(me, -1, &get) == -EINVAL);
    assert(array_get(&get, me, -1) == -EINVAL);
    me = array_destroy(me);
    assert(!me);
}
