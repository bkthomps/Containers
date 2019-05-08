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
int list_size(list me);
int list_is_empty(list me);
void list_copy_to_array(void *arr, list me);

/* Adding */
int list_add_first(list me, void *data);
int list_add_at(list me, int index, void *data);
int list_add_last(list me, void *data);

/* Removing */
int list_remove_first(list me);
int list_remove_at(list me, int index);
int list_remove_last(list me);

/* Setting */
int list_set_first(list me, void *data);
int list_set_at(list me, int index, void *data);
int list_set_last(list me, void *data);

/* Getting */
int list_get_first(void *data, list me);
int list_get_at(void *data, list me, int index);
int list_get_last(void *data, list me);

/* Ending */
void list_clear(list me);
list list_destroy(list me);

#endif /* CONTAINERS_LIST_H */
