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

#include <stdlib.h>
#include "deque.h"
#include "stack.h"

struct internal_stack {
    deque deque_data;
};

/**
 * Initializes a stack.
 *
 * @param data_size the size of each data element in the stack; must be
 *                  positive
 *
 * @return the newly-initialized stack, or NULL if memory allocation error
 */
stack stack_init(const size_t data_size)
{
    struct internal_stack *init;
    if (data_size == 0) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_stack));
    if (!init) {
        return NULL;
    }
    init->deque_data = deque_init(data_size);
    if (!init->deque_data) {
        free(init);
        return NULL;
    }
    return init;
}

/**
 * Determines the size of the stack.
 *
 * @param me the stack to check size of
 *
 * @return the size of the stack
 */
int stack_size(stack me)
{
    return deque_size(me->deque_data);
}

/**
 * Determines if the stack is empty, meaning it contains no elements.
 *
 * @param me the stack to check if empty
 *
 * @return 1 if the stack is empty, otherwise 0
 */
int stack_is_empty(stack me)
{
    return deque_is_empty(me->deque_data);
}

/**
 * Frees unused memory from the stack.
 *
 * @param me the stack to trim
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int stack_trim(stack me)
{
    return deque_trim(me->deque_data);
}

/**
 * Copies the stack to an array.
 *
 * @param arr the initialized array to copy the stack to
 * @param me  the stack to copy to the array
 */
void stack_copy_to_array(void *const arr, stack me)
{
    deque_copy_to_array(arr, me->deque_data);
}

/**
 * Adds an element to the top of the stack.
 *
 * @param me   the stack to add an element to
 * @param data the data to add to the stack
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int stack_push(stack me, void *const data)
{
    return deque_push_back(me->deque_data, data);
}

/**
 * Removes the top element of the stack, and copies the data which is being
 * removed.
 *
 * @param data the copy of the element being removed
 * @param me   the stack to remove the top element from
 *
 * @return 1 if the stack contained elements, otherwise 0
 */
int stack_pop(void *const data, stack me)
{
    return deque_pop_back(data, me->deque_data) == 0;
}

/**
 * Copies the top element of the stack.
 *
 * @param data the copy of the top element of the stack
 * @param me   the stack to copy from
 *
 * @return 1 if the stack contained elements, otherwise 0
 */
int stack_top(void *const data, stack me)
{
    return deque_get_last(data, me->deque_data) == 0;
}

/**
 * Clears the stack and sets it to the state from original initialization.
 *
 * @param me the stack to clear
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int stack_clear(stack me)
{
    return deque_clear(me->deque_data);
}

/**
 * Frees the stack memory. Performing further operations after calling this
 * function results in undefined behavior.
 *
 * @param me the stack to destroy
 *
 * @return NULL
 */
stack stack_destroy(stack me)
{
    deque_destroy(me->deque_data);
    free(me);
    return NULL;
}
