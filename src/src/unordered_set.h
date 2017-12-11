/*
 * Copyright (c) 2017 Bailey Thompson
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

#ifndef CONTAINERS_UNORDERED_SET_H
#define CONTAINERS_UNORDERED_SET_H

#include <stdbool.h>

typedef struct _unordered_set *unordered_set;

// Starting
unordered_set unordered_set_init(size_t key_size,
                                 unsigned long (*hash)(const void *const key),
                                 int (*comparator)(const void *const one,
                                                   const void *const two));

// Utility
int unordered_set_rehash(unordered_set me);
int unordered_set_size(unordered_set me);
bool unordered_set_is_empty(unordered_set me);

// Accessing
int unordered_set_add(unordered_set me, void *key);
bool unordered_set_contains(unordered_set me, void *key);
bool unordered_set_remove(unordered_set me, void *key);

// Ending
void unordered_set_clear(unordered_set me);
unordered_set unordered_set_destroy(unordered_set me);

#endif /* CONTAINERS_UNORDERED_SET_H */
