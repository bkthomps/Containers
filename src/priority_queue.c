/*
 * Copyright (c) 2017-2018 Bailey Thompson
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
#include <memory.h>
#include <errno.h>
#include "vector.h"
#include "priority_queue.h"

#include <stdio.h>
#include <assert.h>

/*
 * Must exactly match the declaration in vector.c
 */
struct _vector {
    size_t data_size;
    int offset;
    int space;
    void *storage;
};

struct _priority_queue {
    vector data;
    size_t data_size;
    int (*comparator)(const void *const one, const void *const two);
};

static void debug_print(priority_queue me, void *const vector_storage)
{
    const int size = vector_size(me->data);
    for (int i = 0; i < size; i++) {
        const int val = *(int *) (vector_storage + i * me->data_size);
        printf("%d, ", val);
        const int left_child = 2 * i + 1;
        const int right_child = 2 * i + 2;
        if (left_child < size) {
            void *left_data = vector_storage + left_child * me->data_size;
            const int left_val = *(int *) left_data;
            assert(val >= left_val);
        }
        if (right_child < size) {
            void *right_data = vector_storage + right_child * me->data_size;
            const int right_val = *(int *) right_data;
            assert(val >= right_val);
        }
    }
    printf("\n");
}

/**
 * Initializes a priority queue, which adapts a container to provide priority
 * queue. Adapts the vector container.
 *
 * @param data_size  The size of the data in the priority queue.
 * @param comparator The priority comparator function.
 *
 * @return The newly-initialized priority queue, or NULL if memory allocation
 *         error.
 */
priority_queue priority_queue_init(const size_t data_size,
                                   int (*comparator)(const void *const,
                                                     const void *const))
{
    struct _priority_queue *const init = malloc(sizeof(struct _priority_queue));
    if (init == NULL) {
        return NULL;
    }
    init->data_size = data_size;
    init->data = vector_init(data_size);
    if (init->data == NULL) {
        free(init);
        return NULL;
    }
    init->comparator = comparator;
    return init;
}

/**
 * Gets the size of the priority queue.
 *
 * @param me The priority queue to check.
 *
 * @return The size of the priority queue.
 */
int priority_queue_size(priority_queue me)
{
    return vector_size(me->data);
}

/**
 * Determines whether or not the priority queue is empty.
 *
 * @param me The priority queue to check.
 *
 * @return If the priority queue is empty.
 */
bool priority_queue_is_empty(priority_queue me)
{
    return vector_is_empty(me->data);
}

/**
 * Adds an element to the priority queue.
 *
 * @param me   The priority queue to add an element to.
 * @param data The data to add to the queue.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int priority_queue_push(priority_queue me, void *const data)
{
    void *const temp = malloc(me->data_size);
    if (temp == NULL) {
        return -ENOMEM;
    }
    const int rc = vector_add_last(me->data, data);
    if (rc != 0) {
        free(temp);
        return rc;
    }
    void *const vector_storage = me->data->storage;
    int index = vector_size(me->data) - 1;
    int parent_index = (index - 1) / 2;
    void *data_index = vector_storage + index * me->data_size;
    void *data_parent_index = vector_storage + parent_index * me->data_size;
    while (index > 0 && me->comparator(data_index, data_parent_index) > 0) {
        memcpy(temp, data_parent_index, me->data_size);
        memcpy(data_parent_index, data_index, me->data_size);
        memcpy(data_index, temp, me->data_size);
        index = parent_index;
        parent_index = (index - 1) / 2;
        data_index = vector_storage + index * me->data_size;
        data_parent_index = vector_storage + parent_index * me->data_size;
    }
    free(temp);
    debug_print(me, vector_storage);
    return 0;
}

/**
 * Removes the highest priority element from the priority queue.
 *
 * @param data The data to have copied from the priority queue.
 * @param me   The priority queue to pop the next element from.
 *
 * @return If the priority queue contained elements.
 */
bool priority_queue_pop(void *const data, priority_queue me)
{
    const int rc = vector_get_first(data, me->data);
    // TODO: now re-order then remove last element
    return rc == 0;
}

/**
 * Gets the highest priority element in the priority queue.
 *
 * @param data Out copy of the highest priority element in the priority queue.
 * @param me   The priority queue to copy from.
 *
 * @return If the priority queue contained elements.
 */
bool priority_queue_front(void *const data, priority_queue me)
{
    return vector_get_first(data, me->data) == 0;
}

/**
 * Clears the elements from the priority queue.
 *
 * @param me The priority queue to clear.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int priority_queue_clear(priority_queue me)
{
    return vector_clear(me->data);
}

/**
 * Frees the priority queue memory.
 *
 * @param me The priority queue to free from memory.
 *
 * @return NULL
 */
priority_queue priority_queue_destroy(priority_queue me)
{
    vector_destroy(me->data);
    free(me);
    return NULL;
}
