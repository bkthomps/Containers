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
#include "deque.h"
#include "queue.h"

static const int TRIM_SIZE = 64;

struct _queue {
    int trim_count;
    deque deque_data;
};

/**
 * Initializes a queue, which adapts a container to provide queue
 * (first-in first-out). Adapts the deque container.
 *
 * @param data_size The size of each element.
 *
 * @return The newly-initialized queue, or NULL if out of memory.
 */
queue queue_init(const size_t data_size)
{
    struct _queue *const init = malloc(sizeof(struct _queue));
    if (init == NULL) {
        return NULL;
    }
    init->trim_count = 0;
    init->deque_data = deque_init(data_size);
    if (init->deque_data == NULL) {
        free(init);
        return NULL;
    }
    return init;
}

/**
 * Determines the size of the queue.
 *
 * @param me The queue to get size of.
 *
 * @return The queue size.
 */
int queue_size(queue me)
{
    return deque_size(me->deque_data);
}

/**
 * Determines if the queue is empty. The queue is empty if it contains no
 * elements.
 *
 * @param me The queue to check if empty.
 *
 * @return If the queue is empty.
 */
bool queue_is_empty(queue me)
{
    return deque_is_empty(me->deque_data);
}

/**
 * Frees the unused memory in the queue.
 *
 * @param me The queue to trim.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int queue_trim(queue me)
{
    return deque_trim(me->deque_data);
}

/**
 * Copies the queue to an array representation.
 *
 * @param arr The array to have copied from the queue.
 * @param me  The queue to copy to the array.
 */
void queue_copy_to_array(void *const arr, queue me)
{
    deque_copy_to_array(arr, me->deque_data);
}

/**
 * Adds an element to the queue.
 *
 * @param me   The queue to add an element to.
 * @param data The data to add to the queue.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int queue_push(queue me, void *const data)
{
    return deque_push_back(me->deque_data, data);
}

/**
 * Removes the next element in the queue and copies the data.
 *
 * @param data The data to have copied from the queue.
 * @param me   The queue to pop the next element from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int queue_pop(void *const data, queue me)
{
    me->trim_count++;
    if (me->trim_count >= TRIM_SIZE) {
        deque_trim(me->deque_data);
        me->trim_count = 0;
    }
    return deque_pop_front(data, me->deque_data);
}

/**
 * Gets the front element of the queue.
 *
 * @param data Copies the front element of the queue.
 * @param me   The queue to copy from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int queue_front(void *const data, queue me)
{
    return deque_get_first(data, me->deque_data);
}

/**
 * Gets the back element of the queue.
 *
 * @param data Copies the back element of the queue.
 * @param me   The queue to copy from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int queue_back(void *const data, queue me)
{
    return deque_get_last(data, me->deque_data);
}

/**
 * Clears the queue and sets it to the original state from initialization.
 *
 * @param me The queue to clear.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int queue_clear(queue me)
{
    return deque_clear(me->deque_data);
}

/**
 * Destroys the queue.
 *
 * @param me The queue to destroy.
 *
 * @return NULL
 */
queue queue_destroy(queue me)
{
    deque_destroy(me->deque_data);
    free(me);
    return NULL;
}
