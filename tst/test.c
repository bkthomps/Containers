#include <stdio.h>
#include "test.h"

#if STUB_MALLOC

#define _GNU_SOURCE
#include <dlfcn.h>

#ifndef RTLD_NEXT
#define RTLD_NEXT ((void *) -1L)
#endif

int fail_malloc = 0;
int fail_calloc = 0;
int fail_realloc = 0;

int delay_fail_malloc = 0;
int delay_fail_calloc = 0;
int delay_fail_realloc = 0;

static void *(*real_malloc)(size_t);
static void *(*real_calloc)(size_t, size_t);
static void *(*real_realloc)(void *, size_t);

void *malloc(size_t size)
{
    if (!real_malloc) {
        *(void **) (&real_malloc) = dlsym(RTLD_NEXT, "malloc");
    }
    if (delay_fail_malloc == 0 && fail_malloc == 1) {
        fail_malloc = 0;
        return NULL;
    }
    if (delay_fail_malloc > 0) {
        delay_fail_malloc--;
    }
    return real_malloc(size);
}

void *calloc(size_t count, size_t size)
{
    if (!real_calloc) {
        *(void **) (&real_calloc) = dlsym(RTLD_NEXT, "calloc");
    }
    if (delay_fail_calloc == 0 && fail_calloc == 1) {
        fail_calloc = 0;
        return NULL;
    }
    if (delay_fail_calloc > 0) {
        delay_fail_calloc--;
    }
    return real_calloc(count, size);
}

void *realloc(void *ptr, size_t new_size)
{
    if (!real_realloc) {
        *(void **) (&real_realloc) = dlsym(RTLD_NEXT, "realloc");
    }
    if (delay_fail_realloc == 0 && fail_realloc == 1) {
        fail_realloc = 0;
        return NULL;
    }
    if (delay_fail_realloc > 0) {
        delay_fail_realloc--;
    }
    return real_realloc(ptr, new_size);
}

#endif /* STUB_MALLOC */

int main(void)
{
    test_array();
    test_vector();
    test_deque();
    test_forward_list();
    test_list();
    test_set();
    test_map();
    test_multiset();
    test_multimap();
    test_unordered_set();
    test_unordered_map();
    test_unordered_multiset();
    test_unordered_multimap();
    test_stack();
    test_queue();
    test_priority_queue();
    printf("Tests Passed\n");
    return 0;
}
