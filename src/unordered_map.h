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

#ifndef CONTAINERS_UNORDERED_MAP_H
#define CONTAINERS_UNORDERED_MAP_H

#include <stdbool.h>

/**
 * The unordered_map data structure, which is a collection of key-value pairs,
 * hashed by keys, keys are unique
 */
typedef struct internal_unordered_map *unordered_map;

// Starting
unordered_map unordered_map_init(size_t key_size,
                                 size_t value_size,
                                 unsigned long (*hash)(const void *const key),
                                 int (*comparator)(const void *const one,
                                                   const void *const two));

// Utility
int unordered_map_rehash(unordered_map me);
int unordered_map_size(unordered_map me);
bool unordered_map_is_empty(unordered_map me);

// Accessing
int unordered_map_put(unordered_map me, void *key, void *value);
bool unordered_map_get(void *value, unordered_map me, void *key);
bool unordered_map_contains(unordered_map me, void *key);
bool unordered_map_remove(unordered_map me, void *key);

// Ending
int unordered_map_clear(unordered_map me);
unordered_map unordered_map_destroy(unordered_map me);

#endif /* CONTAINERS_UNORDERED_MAP_H */
