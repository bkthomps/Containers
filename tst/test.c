#include "test.h"

#define _GNU_SOURCE

#include <dlfcn.h>

#ifndef RTLD_NEXT
#define RTLD_NEXT ((void *) -1L)
#endif

int fail_malloc = 0;
int fail_calloc = 0;
int fail_realloc = 0;

static void *(*real_malloc)(size_t);
static void *(*real_calloc)(size_t, size_t);
static void *(*real_realloc)(void *, size_t);

void *malloc(size_t size)
{
    void *p = NULL;
    if (!real_malloc) {
        real_malloc = dlsym(RTLD_NEXT, "malloc");
    }
    if (!fail_malloc) {
        p = real_malloc(size);
    }
    fail_malloc = 0;
    return p;
}

void *calloc(size_t count, size_t size)
{
    void *p = NULL;
    if (!real_calloc) {
        real_calloc = dlsym(RTLD_NEXT, "calloc");
    }
    if (!fail_calloc) {
        p = real_calloc(count, size);
    }
    fail_calloc = 0;
    return p;
}

void *realloc(void *ptr, size_t new_size)
{
    void *p = NULL;
    if (!real_realloc) {
        real_realloc = dlsym(RTLD_NEXT, "realloc");
    }
    if (!fail_realloc) {
        p = real_realloc(ptr, new_size);
    }
    fail_realloc = 0;
    return p;
}

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
    return 0;
}
