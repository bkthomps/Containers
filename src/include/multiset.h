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

#ifndef BKTHOMPS_CONTAINERS_MULTISET_H
#define BKTHOMPS_CONTAINERS_MULTISET_H

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

#endif /* BKTHOMPS_CONTAINERS_MULTISET_H */
