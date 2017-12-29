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

#ifndef CONTAINERS_SET_H
#define CONTAINERS_SET_H

#include <stdbool.h>

typedef struct _set *set;

// Starting
set set_init(size_t key_size,
             int (*comparator)(const void *const one, const void *const two));

// Capacity
int set_size(set me);
bool set_is_empty(set me);

// Accessing
int set_add(set me, void *key, int /* TODO: remove this param */);
bool set_contains(set me, void *key, int /* TODO: remove this param */);
bool set_remove(set me, void *key, int /* TODO: remove this param */);

// Ending
void set_clear(set me, int /* TODO: remove this param */);
set set_destroy(set me);

#endif /* CONTAINERS_SET_H */
