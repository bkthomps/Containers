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

#ifndef CONTAINERS_VECTOR_H
#define CONTAINERS_VECTOR_H

#include <stdbool.h>

typedef struct _vector *vector;

// Starting
vector vector_init(size_t data_size);
// Utility
int vector_size(vector me);
bool vector_is_empty(vector me);
int vector_reserve(vector me, int size);
int vector_trim(vector me);
void vector_to_array(void *array, vector me);
// Adding
int vector_add_first(vector me, void *data);
int vector_add_at(vector me, int index, void *data);
int vector_add_last(vector me, void *data);
// Removing
int vector_remove_first(vector me);
int vector_remove_at(vector me, int index);
int vector_remove_last(vector me);
// Setting
int vector_set_first(vector me, void *data);
int vector_set_at(vector me, int index, void *data);
int vector_set_last(vector me, void *data);
// Getting
int vector_get_first(void *data, vector me);
int vector_get_at(void *data, vector me, int index);
int vector_get_last(void *data, vector me);
// Ending
int vector_clear(vector me);
vector vector_destroy(vector me);

#endif /* CONTAINERS_VECTOR_H */
