#ifndef CONTAINERS_TEST_H
#define CONTAINERS_TEST_H

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#define STUB_MALLOC 1

#if STUB_MALLOC

extern int fail_malloc;
extern int fail_calloc;
extern int fail_realloc;

extern int delay_fail_malloc;
extern int delay_fail_calloc;
extern int delay_fail_realloc;

#endif /* STUB_MALLOC */

void test_array(void);
void test_vector(void);
void test_deque(void);
void test_forward_list(void);
void test_list(void);
void test_set(void);
void test_map(void);
void test_multiset(void);
void test_multimap(void);
void test_unordered_set(void);
void test_unordered_map(void);
void test_unordered_multiset(void);
void test_unordered_multimap(void);
void test_stack(void);
void test_queue(void);
void test_priority_queue(void);

#endif /* CONTAINERS_TEST_H */
