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
#include "vector.h"
#include "priority_queue.h"

struct _priority_queue {
    vector data;
    int (*comparator)(const void *const one, const void *const two);
};

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

int priority_queue_push(priority_queue me, void *const data)
{
    vector_add_last(me->data, data);
    // TODO: added it, now check if needs re-order
    return 0;
}

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
