#include "test.h"
#include "../src/priority_queue.h"

static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}

void test_priority_queue(void)
{
    // TODO
}
