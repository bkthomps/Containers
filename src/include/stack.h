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

#ifndef BKTHOMPS_CONTAINERS_STACK_H
#define BKTHOMPS_CONTAINERS_STACK_H

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

#endif /* BKTHOMPS_CONTAINERS_STACK_H */
