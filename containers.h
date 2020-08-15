/*
 * Copyright (c) 2017-2020 Bailey Thompson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * The Containers library is hosted at: github.com/bkthomps/Containers
 * The author is: Bailey Thompson
 * This local version is: v1.1.2
 */


#ifndef CONTAINERS_UNORDERED_MAP_H
#define CONTAINERS_UNORDERED_MAP_H

#include <stdlib.h>

/**
 * The unordered_map data structure, which is a collection of key-value pairs,
 * hashed by keys, keys are unique
 */
typedef struct internal_unordered_map *unordered_map;

/* Starting */
unordered_map unordered_map_init(size_t key_size,
                                 size_t value_size,
                                 unsigned long (*hash)(const void *const key),
                                 int (*comparator)(const void *const one,
                                                   const void *const two));

/* Utility */
int unordered_map_rehash(unordered_map me);
size_t unordered_map_size(unordered_map me);
int unordered_map_is_empty(unordered_map me);

/* Accessing */
int unordered_map_put(unordered_map me, void *key, void *value);
int unordered_map_get(void *value, unordered_map me, void *key);
int unordered_map_contains(unordered_map me, void *key);
int unordered_map_remove(unordered_map me, void *key);

/* Ending */
int unordered_map_clear(unordered_map me);
unordered_map unordered_map_destroy(unordered_map me);

#endif /* CONTAINERS_UNORDERED_MAP_H */


#ifndef CONTAINERS_MAP_H
#define CONTAINERS_MAP_H

#include <stdlib.h>

/**
 * The map data structure, which is a collection of key-value pairs, sorted by
 * keys, keys are unique.
 */
typedef struct internal_map *map;

/* Starting */
map map_init(size_t key_size,
             size_t value_size,
             int (*comparator)(const void *const one, const void *const two));

/* Capacity */
int map_size(map me);
int map_is_empty(map me);

/* Accessing */
int map_put(map me, void *key, void *value);
int map_get(void *value, map me, void *key);
int map_contains(map me, void *key);
int map_remove(map me, void *key);

/* Ending */
void map_clear(map me);
map map_destroy(map me);

#endif /* CONTAINERS_MAP_H */


#ifndef CONTAINERS_UNORDERED_MULTISET_H
#define CONTAINERS_UNORDERED_MULTISET_H

#include <stdlib.h>

/**
 * The unordered_multiset data structure, which is a collection of keys, hashed
 * by keys.
 */
typedef struct internal_unordered_multiset *unordered_multiset;

/* Starting */
unordered_multiset
unordered_multiset_init(size_t key_size,
                        unsigned long (*hash)(const void *const key),
                        int (*comparator)(const void *const one,
                                          const void *const two));

/* Utility */
int unordered_multiset_rehash(unordered_multiset me);
size_t unordered_multiset_size(unordered_multiset me);
int unordered_multiset_is_empty(unordered_multiset me);

/* Accessing */
int unordered_multiset_put(unordered_multiset me, void *key);
size_t unordered_multiset_count(unordered_multiset me, void *key);
int unordered_multiset_contains(unordered_multiset me, void *key);
int unordered_multiset_remove(unordered_multiset me, void *key);
int unordered_multiset_remove_all(unordered_multiset me, void *key);

/* Ending */
int unordered_multiset_clear(unordered_multiset me);
unordered_multiset unordered_multiset_destroy(unordered_multiset me);

#endif /* CONTAINERS_UNORDERED_MULTISET_H */


#ifndef CONTAINERS_MULTIMAP_H
#define CONTAINERS_MULTIMAP_H

#include <stdlib.h>

/**
 * The multimap data structure, which is a collection of key-value pairs, sorted
 * by keys.
 */
typedef struct internal_multimap *multimap;

/* Starting */
multimap multimap_init(size_t key_size,
                       size_t value_size,
                       int (*key_comparator)(const void *const one,
                                             const void *const two),
                       int (*value_comparator)(const void *const one,
                                               const void *const two));

/* Capacity */
int multimap_size(multimap me);
int multimap_is_empty(multimap me);

/* Accessing */
int multimap_put(multimap me, void *key, void *value);
void multimap_get_start(multimap me, void *key);
int multimap_get_next(void *value, multimap me);
int multimap_count(multimap me, void *key);
int multimap_contains(multimap me, void *key);
int multimap_remove(multimap me, void *key, void *value);
int multimap_remove_all(multimap me, void *key);

/* Ending */
void multimap_clear(multimap me);
multimap multimap_destroy(multimap me);

#endif /* CONTAINERS_MULTIMAP_H */


#ifndef CONTAINERS_PRIORITY_QUEUE_H
#define CONTAINERS_PRIORITY_QUEUE_H

#include <stdlib.h>

/**
 * The priority_queue data structure, which adapts a container to provide a
 * priority queue. Adapts the vector container.
 */
typedef struct internal_priority_queue *priority_queue;

/* Starting */
priority_queue priority_queue_init(size_t data_size,
                                   int (*comparator)(const void *const one,
                                                     const void *const two));

/* Utility */
size_t priority_queue_size(priority_queue me);
int priority_queue_is_empty(priority_queue me);

/* Adding */
int priority_queue_push(priority_queue me, void *data);

/* Removing */
int priority_queue_pop(void *data, priority_queue me);

/* Getting */
int priority_queue_front(void *data, priority_queue me);

/* Ending */
int priority_queue_clear(priority_queue me);
priority_queue priority_queue_destroy(priority_queue me);

#endif /* CONTAINERS_PRIORITY_QUEUE_H */


#ifndef CONTAINERS_LIST_H
#define CONTAINERS_LIST_H

#include <stdlib.h>

/**
 * The list data structure, which is a doubly-linked list.
 */
typedef struct internal_list *list;

/* Starting */
list list_init(size_t data_size);

/* Utility */
size_t list_size(list me);
int list_is_empty(list me);
void list_copy_to_array(void *arr, list me);

/* Adding */
int list_add_first(list me, void *data);
int list_add_at(list me, size_t index, void *data);
int list_add_last(list me, void *data);

/* Removing */
int list_remove_first(list me);
int list_remove_at(list me, size_t index);
int list_remove_last(list me);

/* Setting */
int list_set_first(list me, void *data);
int list_set_at(list me, size_t index, void *data);
int list_set_last(list me, void *data);

/* Getting */
int list_get_first(void *data, list me);
int list_get_at(void *data, list me, size_t index);
int list_get_last(void *data, list me);

/* Ending */
void list_clear(list me);
list list_destroy(list me);

#endif /* CONTAINERS_LIST_H */


#ifndef CONTAINERS_UNORDERED_MULTIMAP_H
#define CONTAINERS_UNORDERED_MULTIMAP_H

#include <stdlib.h>

/**
 * The unordered_multimap data structure, which is a collection of key-value
 * pairs, hashed by keys.
 */
typedef struct internal_unordered_multimap *unordered_multimap;

/* Starting */
unordered_multimap
unordered_multimap_init(size_t key_size,
                        size_t value_size,
                        unsigned long (*hash)(const void *const key),
                        int (*key_comparator)(const void *const one,
                                              const void *const two),
                        int (*value_comparator)(const void *const one,
                                                const void *const two));

/* Utility */
int unordered_multimap_rehash(unordered_multimap me);
size_t unordered_multimap_size(unordered_multimap me);
int unordered_multimap_is_empty(unordered_multimap me);

/* Accessing */
int unordered_multimap_put(unordered_multimap me, void *key, void *value);
void unordered_multimap_get_start(unordered_multimap me, void *key);
int unordered_multimap_get_next(void *value, unordered_multimap me);
int unordered_multimap_count(unordered_multimap me, void *key);
int unordered_multimap_contains(unordered_multimap me, void *key);
int unordered_multimap_remove(unordered_multimap me, void *key, void *value);
int unordered_multimap_remove_all(unordered_multimap me, void *key);

/* Ending */
int unordered_multimap_clear(unordered_multimap me);
unordered_multimap unordered_multimap_destroy(unordered_multimap me);

#endif /* CONTAINERS_UNORDERED_MULTIMAP_H */


#ifndef CONTAINERS_ARRAY_H
#define CONTAINERS_ARRAY_H

#include <stdlib.h>

/**
 * The array data structure, which is a static contiguous array.
 */
typedef char *array;

/* Starting */
array array_init(size_t element_count, size_t data_size);

/* Utility */
size_t array_size(array me);
void array_copy_to_array(void *arr, array me);
void *array_get_data(array me);

/* Accessing */
int array_set(array me, size_t index, void *data);
int array_get(void *data, array me, size_t index);

/* Ending */
array array_destroy(array me);

#endif /* CONTAINERS_ARRAY_H */


#ifndef CONTAINERS_FORWARD_LIST_H
#define CONTAINERS_FORWARD_LIST_H

#include <stdlib.h>

/**
 * The forward_list data structure, which is a singly-linked list.
 */
typedef struct internal_forward_list *forward_list;

/* Starting */
forward_list forward_list_init(size_t data_size);

/* Utility */
size_t forward_list_size(forward_list me);
int forward_list_is_empty(forward_list me);
void forward_list_copy_to_array(void *arr, forward_list me);

/* Adding */
int forward_list_add_first(forward_list me, void *data);
int forward_list_add_at(forward_list me, size_t index, void *data);
int forward_list_add_last(forward_list me, void *data);

/* Removing */
int forward_list_remove_first(forward_list me);
int forward_list_remove_at(forward_list me, size_t index);
int forward_list_remove_last(forward_list me);

/* Setting */
int forward_list_set_first(forward_list me, void *data);
int forward_list_set_at(forward_list me, size_t index, void *data);
int forward_list_set_last(forward_list me, void *data);

/* Getting */
int forward_list_get_first(void *data, forward_list me);
int forward_list_get_at(void *data, forward_list me, size_t index);
int forward_list_get_last(void *data, forward_list me);

/* Ending */
void forward_list_clear(forward_list me);
forward_list forward_list_destroy(forward_list me);

#endif /* CONTAINERS_FORWARD_LIST_H */


#ifndef CONTAINERS_MULTISET_H
#define CONTAINERS_MULTISET_H

#include <stdlib.h>

/**
 * The multiset data structure, which is a collection of key-value pairs, sorted
 * by keys, keys are unique
 */
typedef struct internal_multiset *multiset;

/* Starting */
multiset multiset_init(size_t key_size,
                       int (*comparator)(const void *const one,
                                         const void *const two));

/* Capacity */
size_t multiset_size(multiset me);
int multiset_is_empty(multiset me);

/* Accessing */
int multiset_put(multiset me, void *key);
size_t multiset_count(multiset me, void *key);
int multiset_contains(multiset me, void *key);
int multiset_remove(multiset me, void *key);
int multiset_remove_all(multiset me, void *key);

/* Ending */
void multiset_clear(multiset me);
multiset multiset_destroy(multiset me);

#endif /* CONTAINERS_MULTISET_H */


#ifndef CONTAINERS_DEQUE_H
#define CONTAINERS_DEQUE_H

#include <stdlib.h>

/**
 * The deque data structure, which is a doubly-ended queue.
 */
typedef struct internal_deque *deque;

/* Starting */
deque deque_init(size_t data_size);

/* Utility */
size_t deque_size(deque me);
int deque_is_empty(deque me);
int deque_trim(deque me);
void deque_copy_to_array(void *arr, deque me);

/* Adding */
int deque_push_front(deque me, void *data);
int deque_push_back(deque me, void *data);

/* Removing */
int deque_pop_front(void *data, deque me);
int deque_pop_back(void *data, deque me);

/* Setting */
int deque_set_first(deque me, void *data);
int deque_set_at(deque me, size_t index, void *data);
int deque_set_last(deque me, void *data);

/* Getting */
int deque_get_first(void *data, deque me);
int deque_get_at(void *data, deque me, size_t index);
int deque_get_last(void *data, deque me);

/* Ending */
int deque_clear(deque me);
deque deque_destroy(deque me);

#endif /* CONTAINERS_DEQUE_H */


#ifndef CONTAINERS_STACK_H
#define CONTAINERS_STACK_H

#include <stdlib.h>

/**
 * The stack data structure, which adapts a container to provide a stack
 * (last-in first-out). Adapts the deque container.
 */
typedef struct internal_deque *stack;

/* Starting */
stack stack_init(size_t data_size);

/* Utility */
size_t stack_size(stack me);
int stack_is_empty(stack me);
int stack_trim(stack me);
void stack_copy_to_array(void *arr, stack me);

/* Adding */
int stack_push(stack me, void *data);

/* Removing */
int stack_pop(void *data, stack me);

/* Getting */
int stack_top(void *data, stack me);

/* Ending */
int stack_clear(stack me);
stack stack_destroy(stack me);

#endif /* CONTAINERS_STACK_H */


#ifndef CONTAINERS_UNORDERED_SET_H
#define CONTAINERS_UNORDERED_SET_H

#include <stdlib.h>

/**
 * The unordered_set data structure, which is a collection of unique keys,
 * hashed by keys.
 */
typedef struct internal_unordered_set *unordered_set;

/* Starting */
unordered_set unordered_set_init(size_t key_size,
                                 unsigned long (*hash)(const void *const key),
                                 int (*comparator)(const void *const one,
                                                   const void *const two));

/* Utility */
int unordered_set_rehash(unordered_set me);
size_t unordered_set_size(unordered_set me);
int unordered_set_is_empty(unordered_set me);

/* Accessing */
int unordered_set_put(unordered_set me, void *key);
int unordered_set_contains(unordered_set me, void *key);
int unordered_set_remove(unordered_set me, void *key);

/* Ending */
int unordered_set_clear(unordered_set me);
unordered_set unordered_set_destroy(unordered_set me);

#endif /* CONTAINERS_UNORDERED_SET_H */


#ifndef CONTAINERS_VECTOR_H
#define CONTAINERS_VECTOR_H

#include <stdlib.h>

/**
 * The vector data structure, which is a dynamic contiguous array.
 */
typedef struct internal_vector *vector;

/* Starting */
vector vector_init(size_t data_size);

/* Utility */
size_t vector_size(vector me);
size_t vector_capacity(vector me);
int vector_is_empty(vector me);
int vector_reserve(vector me, size_t size);
int vector_trim(vector me);
void vector_copy_to_array(void *arr, vector me);
void *vector_get_data(vector me);

/* Adding */
int vector_add_first(vector me, void *data);
int vector_add_at(vector me, size_t index, void *data);
int vector_add_last(vector me, void *data);

/* Removing */
int vector_remove_first(vector me);
int vector_remove_at(vector me, size_t index);
int vector_remove_last(vector me);

/* Setting */
int vector_set_first(vector me, void *data);
int vector_set_at(vector me, size_t index, void *data);
int vector_set_last(vector me, void *data);

/* Getting */
int vector_get_first(void *data, vector me);
int vector_get_at(void *data, vector me, size_t index);
int vector_get_last(void *data, vector me);

/* Ending */
int vector_clear(vector me);
vector vector_destroy(vector me);

#endif /* CONTAINERS_VECTOR_H */


#ifndef CONTAINERS_QUEUE_H
#define CONTAINERS_QUEUE_H

#include <stdlib.h>

/**
 * The queue data structure, which adapts a container to provide a queue
 * (first-in first-out). Adapts the deque container.
 */
typedef struct internal_deque *queue;

/* Starting */
queue queue_init(size_t data_size);

/* Utility */
size_t queue_size(queue me);
int queue_is_empty(queue me);
int queue_trim(queue me);
void queue_copy_to_array(void *arr, queue me);

/* Adding */
int queue_push(queue me, void *data);

/* Removing */
int queue_pop(void *data, queue me);

/* Getting */
int queue_front(void *data, queue me);
int queue_back(void *data, queue me);

/* Ending */
int queue_clear(queue me);
queue queue_destroy(queue me);

#endif /* CONTAINERS_QUEUE_H */


#ifndef CONTAINERS_SET_H
#define CONTAINERS_SET_H

#include <stdlib.h>

/**
 * The set data structure, which is a collection of unique keys, sorted by keys.
 */
typedef struct internal_set *set;

/* Starting */
set set_init(size_t key_size,
             int (*comparator)(const void *const one, const void *const two));

/* Capacity */
size_t set_size(set me);
int set_is_empty(set me);

/* Accessing */
int set_put(set me, void *key);
int set_contains(set me, void *key);
int set_remove(set me, void *key);

/* Ending */
void set_clear(set me);
set set_destroy(set me);

#endif /* CONTAINERS_SET_H */
