#include "test.h"
#include "../src/array.h"

void test_array(void)
{
    array a = array_init(10, sizeof(int));
    assert(a != NULL);
    assert(array_size(a) == 10);
    for (int i = 0; i < 10; i++) {
        int g = 0xdeadbeef;
        array_get(&g, a, i);
        assert(g == 0);
    }
    for (int i = 0; i < 10; i++) {
        int g = 0xdeadbeef;
        array_set(a, i, &i);
        array_get(&g, a, i);
        assert(g == i);
    }
    for (int i = 0; i < 10; i++) {
        int g = 0xdeadbeef;
        array_get(&g, a, i);
        assert(g == i);
    }
    int arr[10] = {0};
    array_copy_to_array(arr, a);
    for (int i = 0; i < 10; i++) {
        assert(arr[i] == i);
    }
    int *data = array_get_data(a);
    for (int i = 0; i < 10; i++) {
        assert(data[i] == i);
    }
    a = array_destroy(a);
    assert(a == NULL);
}
