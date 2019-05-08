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

#ifndef CONTAINERS_FORWARD_LIST_H
#define CONTAINERS_FORWARD_LIST_H

/**
 * The forward_list data structure, which is a singly-linked list.
 */
typedef struct internal_forward_list *forward_list;

/* Starting */
forward_list forward_list_init(size_t data_size);

/* Utility */
int forward_list_size(forward_list me);
int forward_list_is_empty(forward_list me);
void forward_list_copy_to_array(void *arr, forward_list me);

/* Adding */
int forward_list_add_first(forward_list me, void *data);
int forward_list_add_at(forward_list me, int index, void *data);
int forward_list_add_last(forward_list me, void *data);

/* Removing */
int forward_list_remove_first(forward_list me);
int forward_list_remove_at(forward_list me, int index);
int forward_list_remove_last(forward_list me);

/* Setting */
int forward_list_set_first(forward_list me, void *data);
int forward_list_set_at(forward_list me, int index, void *data);
int forward_list_set_last(forward_list me, void *data);

/* Getting */
int forward_list_get_first(void *data, forward_list me);
int forward_list_get_at(void *data, forward_list me, int index);
int forward_list_get_last(void *data, forward_list me);

/* Ending */
void forward_list_clear(forward_list me);
forward_list forward_list_destroy(forward_list me);

#endif /* CONTAINERS_FORWARD_LIST_H */
