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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "vector.h"
#include "priority_queue.h"

struct internal_priority_queue {
    vector data;
    size_t data_size;
    int (*comparator)(const void *const one, const void *const two);
};

/**
 * Initializes a priority queue.
 *
 * @param data_size  the size of the data in the priority queue; must be
 *                   positive
 * @param comparator the priority comparator function; must not be NULL
 *
 * @return the newly-initialized priority queue, or NULL if memory allocation
 *         error
 */
priority_queue priority_queue_init(const size_t data_size,
                                   int (*comparator)(const void *const,
                                                     const void *const))
{
    struct internal_priority_queue *init;
    if (data_size == 0 || !comparator) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_priority_queue));
    if (!init) {
        return NULL;
    }
    init->data_size = data_size;
    init->data = vector_init(data_size);
    if (!init->data) {
        free(init);
        return NULL;
    }
    init->comparator = comparator;
    return init;
}

/**
 * Gets the size of the priority queue.
 *
 * @param me the priority queue to check
 *
 * @return the size of the priority queue
 */
int priority_queue_size(priority_queue me)
{
    return vector_size(me->data);
}

/**
 * Determines whether or not the priority queue is empty.
 *
 * @param me the priority queue to check
 *
 * @return 1 if the priority queue is empty, otherwise 0
 */
int priority_queue_is_empty(priority_queue me)
{
    return vector_is_empty(me->data);
}

/**
 * Adds an element to the priority queue.
 *
 * @param me   the priority queue to add an element to
 * @param data the data to add to the queue
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int priority_queue_push(priority_queue me, void *const data)
{
    int rc;
    void *vector_storage;
    int index;
    int parent_index;
    void *data_index;
    void *data_parent_index;
    void *const temp = malloc(me->data_size);
    if (!temp) {
        return -ENOMEM;
    }
    rc = vector_add_last(me->data, data);
    if (rc != 0) {
        free(temp);
        return rc;
    }
    vector_storage = vector_get_data(me->data);
    index = vector_size(me->data) - 1;
    parent_index = (index - 1) / 2;
    data_index = (char *) vector_storage + index * me->data_size;
    data_parent_index = (char *) vector_storage + parent_index * me->data_size;
    while (index > 0 && me->comparator(data_index, data_parent_index) > 0) {
        memcpy(temp, data_parent_index, me->data_size);
        memcpy(data_parent_index, data_index, me->data_size);
        memcpy(data_index, temp, me->data_size);
        index = parent_index;
        parent_index = (index - 1) / 2;
        data_index = (char *) vector_storage + index * me->data_size;
        data_parent_index =
                (char *) vector_storage + parent_index * me->data_size;
    }
    free(temp);
    return 0;
}

/**
 * Removes the highest priority element from the priority queue.
 *
 * @param data the data to have copied from the priority queue
 * @param me   the priority queue to pop the next element from
 *
 * @return 1 if the priority queue contained elements, otherwise 0
 */
int priority_queue_pop(void *const data, priority_queue me)
{
    void *vector_storage;
    int size;
    void *temp;
    int index;
    int left_index;
    int right_index;
    void *data_index;
    void *data_left_index;
    void *data_right_index;
    const int rc = vector_get_first(data, me->data);
    if (rc != 0) {
        return 0;
    }
    vector_storage = vector_get_data(me->data);
    size = vector_size(me->data) - 1;
    temp = (char *) vector_storage + size * me->data_size;
    memcpy(vector_storage, temp, me->data_size);
    left_index = 1;
    right_index = 2;
    data_index = vector_storage;
    data_left_index = (char *) vector_storage + left_index * me->data_size;
    data_right_index = (char *) vector_storage + right_index * me->data_size;
    for (;;) {
        if (right_index < size &&
            me->comparator(data_right_index, data_left_index) > 0 &&
            me->comparator(data_right_index, data_index) > 0) {
            /* Swap parent and right child then continue down right child. */
            memcpy(temp, data_index, me->data_size);
            memcpy(data_index, data_right_index, me->data_size);
            memcpy(data_right_index, temp, me->data_size);
            index = right_index;
        } else if (left_index < size &&
                   me->comparator(data_left_index, data_index) > 0) {
            /* Swap parent and left child then continue down left child. */
            memcpy(temp, data_index, me->data_size);
            memcpy(data_index, data_left_index, me->data_size);
            memcpy(data_left_index, temp, me->data_size);
            index = left_index;
        } else {
            break;
        }
        left_index = 2 * index + 1;
        right_index = 2 * index + 2;
        data_index = (char *) vector_storage + index * me->data_size;
        data_left_index = (char *) vector_storage + left_index * me->data_size;
        data_right_index =
                (char *) vector_storage + right_index * me->data_size;
    }
    vector_remove_last(me->data);
    return 1;
}

/**
 * Gets the highest priority element in the priority queue.
 *
 * @param data the out copy of the highest priority element in the priority
 *             queue
 * @param me   the priority queue to copy from
 *
 * @return 1 if the priority queue contained elements, otherwise 0
 */
int priority_queue_front(void *const data, priority_queue me)
{
    return vector_get_first(data, me->data) == 0;
}

/**
 * Clears the elements from the priority queue.
 *
 * @param me the priority queue to clear
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int priority_queue_clear(priority_queue me)
{
    return vector_clear(me->data);
}

/**
 * Frees the priority queue memory.
 *
 * @param me the priority queue to free from memory
 *
 * @return NULL
 */
priority_queue priority_queue_destroy(priority_queue me)
{
    vector_destroy(me->data);
    free(me);
    return NULL;
}
