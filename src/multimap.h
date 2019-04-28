/*
 * Copyright (c) 2017-2019 Bailey Thompson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CONTAINERS_MULTIMAP_H
#define CONTAINERS_MULTIMAP_H

#include <stdbool.h>

/**
 * The multimap data structure, which is a collection of key-value pairs, sorted
 * by keys.
 */
typedef struct internal_multimap *multimap;

// Starting
multimap multimap_init(size_t key_size,
                       size_t value_size,
                       int (*key_comparator)(const void *const one,
                                             const void *const two),
                       int (*value_comparator)(const void *const one,
                                               const void *const two));

// Capacity
int multimap_size(multimap me);
bool multimap_is_empty(multimap me);

// Accessing
int multimap_put(multimap me, void *key, void *value);
void multimap_get_start(multimap me, void *key);
bool multimap_get_next(void *value, multimap me);
int multimap_count(multimap me, void *key);
bool multimap_contains(multimap me, void *key);
bool multimap_remove(multimap me, void *key, void *value);
bool multimap_remove_all(multimap me, void *key);

// Ending
void multimap_clear(multimap me);
multimap multimap_destroy(multimap me);

#endif /* CONTAINERS_MULTIMAP_H */
