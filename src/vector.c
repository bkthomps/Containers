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

static const int START_SPACE = 8;
static const double RESIZE_RATIO = 1.5;

struct internal_vector {
    size_t bytes_per_item;
    int item_count;
    int item_capacity;
    void *data;
};

/**
 * Initializes a vector.
 *
 * @param data_size the size of each element in the vector; must be positive
 *
 * @return the newly-initialized vector, or NULL if memory allocation error
 */
vector vector_init(const size_t data_size)
{
    struct internal_vector *init;
    if (data_size == 0) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_vector));
    if (!init) {
        return NULL;
    }
    init->bytes_per_item = data_size;
    init->item_count = 0;
    init->item_capacity = START_SPACE;
    init->data = malloc(init->item_capacity * init->bytes_per_item);
    if (!init->data) {
        free(init);
        return NULL;
    }
    return init;
}

/**
 * Gets the size being used by the vector.
 *
 * @param me the vector to check
 *
 * @return the size being used by the vector
 */
int vector_size(vector me)
{
    return me->item_count;
}

/**
 * Gets the capacity that the internal storage of the vector is using.
 *
 * @param me the vector to check
 *
 * @return the capacity that the internal storage of the vector is using
 */
int vector_capacity(vector me)
{
    return me->item_capacity;
}

/**
 * Determines whether or not the vector is empty.
 *
 * @param me the vector to check
 *
 * @return 1 if the vector is empty, otherwise 0
 */
int vector_is_empty(vector me)
{
    return vector_size(me) == 0;
}

/*
 * Sets the space of the buffer. Assumes that size is at least the same as the
 * amount of items currently in the vector.
 */
static int vector_set_space(vector me, const int size)
{
    void *const temp = realloc(me->data, size * me->bytes_per_item);
    if (!temp) {
        return -ENOMEM;
    }
    me->data = temp;
    me->item_capacity = size;
    return 0;
}

/**
 * Reserves space specified. If more space than specified is already reserved,
 * then the previous space will be kept.
 *
 * @param me   the vector to reserve space for
 * @param size the space to reserve
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int vector_reserve(vector me, int size)
{
    if (me->item_capacity >= size) {
        return 0;
    }
    return vector_set_space(me, size);
}

/**
 * Sets the size of the vector buffer to the current size being used.
 *
 * @param me the vector to trim
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int vector_trim(vector me)
{
    return vector_set_space(me, me->item_count);
}

/**
 * Copies the vector to an array.
 *
 * @param arr the initialized array to copy the vector to
 * @param me  the vector to copy to the array
 */
void vector_copy_to_array(void *const arr, vector me)
{
    memcpy(arr, me->data, me->item_count * me->bytes_per_item);
}

/**
 * Gets the storage element of the vector which is contiguous in memory. If the
 * data is modified, the data in the vector is modified. Also, any vector
 * operation may invalidate this data pointer. The vector owns the data pointer,
 * thus it must not be freed.
 *
 * @param me the vector to get the storage element from
 *
 * @return the storage element of the vector
 */
void *vector_get_data(vector me)
{
    return me->data;
}

/**
 * Adds an element to the start of the vector.
 *
 * @param me   the vector to add to
 * @param data the data to add to the vector
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int vector_add_first(vector me, void *const data)
{
    return vector_add_at(me, 0, data);
}

/**
 * Adds an element to the location specified.
 *
 * @param me    the vector to add to
 * @param index the location in the vector to add the data to
 * @param data  the data to add to the vector
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 * @return -EINVAL if invalid argument
 */
int vector_add_at(vector me, const int index, void *const data)
{
    if (index < 0 || index > me->item_count) {
        return -EINVAL;
    }
    if (me->item_count + 1 >= me->item_capacity) {
        const int new_space = (int) (me->item_capacity * RESIZE_RATIO);
        void *const temp = realloc(me->data, new_space * me->bytes_per_item);
        if (!temp) {
            return -ENOMEM;
        }
        me->data = temp;
        me->item_capacity = new_space;
    }
    if (index != me->item_count) {
        memmove((char *) me->data + (index + 1) * me->bytes_per_item,
                (char *) me->data + index * me->bytes_per_item,
                (me->item_count - index) * me->bytes_per_item);
    }
    memcpy((char *) me->data + index * me->bytes_per_item, data,
           me->bytes_per_item);
    me->item_count++;
    return 0;
}

/**
 * Adds an element to the end of the vector.
 *
 * @param me   the vector to add to
 * @param data the data to add to the vector
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int vector_add_last(vector me, void *const data)
{
    return vector_add_at(me, me->item_count, data);
}

/*
 * Determines if the input is illegal.
 */
static int vector_is_illegal_input(vector me, const int index)
{
    return index < 0 || index >= me->item_count;
}

/**
 * Removes the first element from the vector.
 *
 * @param me the vector to remove from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_remove_first(vector me)
{
    return vector_remove_at(me, 0);
}

/**
 * Removes element based on its index.
 *
 * @param me    the vector to remove from
 * @param index the location in the vector to remove the data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_remove_at(vector me, const int index)
{
    if (vector_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    me->item_count--;
    memmove((char *) me->data + index * me->bytes_per_item,
            (char *) me->data + (index + 1) * me->bytes_per_item,
            (me->item_count - index) * me->bytes_per_item);
    return 0;
}

/**
 * Removes the last element from the vector.
 *
 * @param me the vector to remove from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_remove_last(vector me)
{
    if (me->item_count == 0) {
        return -EINVAL;
    }
    me->item_count--;
    return 0;
}

/**
 * Sets the data for the first element in the vector.
 *
 * @param me   the vector to set data for
 * @param data the data to set at the start of the vector
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_set_first(vector me, void *const data)
{
    return vector_set_at(me, 0, data);
}

/**
 * Sets the data for a specified element in the vector.
 *
 * @param me    the vector to set data for
 * @param index the location to set data at in the vector
 * @param data  the data to set at the location in the vector
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_set_at(vector me, const int index, void *const data)
{
    if (vector_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    memcpy((char *) me->data + index * me->bytes_per_item, data,
           me->bytes_per_item);
    return 0;
}

/**
 * Sets the data for the last element in the vector.
 *
 * @param me   the vector to set data for
 * @param data the data to set at the end of the vector
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_set_last(vector me, void *const data)
{
    return vector_set_at(me, me->item_count - 1, data);
}

/**
 * Copies the first element of the vector to data.
 *
 * @param data the data to copy to
 * @param me   the vector to copy from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_get_first(void *const data, vector me)
{
    return vector_get_at(data, me, 0);
}

/**
 * Copies the element at index of the vector to data.
 *
 * @param data  the data to copy to
 * @param me    the vector to copy from
 * @param index the index to copy from in the vector
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_get_at(void *const data, vector me, const int index)
{
    if (vector_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    memcpy(data, (char *) me->data + index * me->bytes_per_item,
           me->bytes_per_item);
    return 0;
}

/**
 * Copies the last element of the vector to data.
 *
 * @param data the data to copy to
 * @param me   the vector to copy from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int vector_get_last(void *const data, vector me)
{
    return vector_get_at(data, me, me->item_count - 1);
}

/**
 * Clears the elements from the vector.
 *
 * @param me the vector to clear
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int vector_clear(vector me)
{
    me->item_count = 0;
    return vector_set_space(me, START_SPACE);
}

/**
 * Frees the vector memory. Performing further operations after calling this
 * function results in undefined behavior.
 *
 * @param me the vector to free from memory
 *
 * @return NULL
 */
vector vector_destroy(vector me)
{
    free(me->data);
    me->data = NULL;
    free(me);
    return NULL;
}
