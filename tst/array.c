#include "test.h"
#include "../src/array.h"

void test_array(void)
{
    array me = array_init(10, sizeof(int));
    assert(me);
    assert(array_size(me) == 10);
    for (int i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_get(&get, me, i);
        assert(get == 0);
    }
    for (int i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_set(me, i, &i);
        array_get(&get, me, i);
        assert(get == i);
    }
    for (int i = 0; i < 10; i++) {
        int get = 0xdeadbeef;
        array_get(&get, me, i);
        assert(get == i);
    }
    int arr[10] = {0};
    array_copy_to_array(arr, me);
    for (int i = 0; i < 10; i++) {
        assert(arr[i] == i);
    }
    int *const data = array_get_data(me);
    for (int i = 0; i < 10; i++) {
        assert(data[i] == i);
    }
    me = array_destroy(me);
    assert(!me);
}
