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

#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include "array.h"

struct _array {
    size_t data_size;
    int element_count;
    void *data;
};

/**
 * Initializes an array, which is a static contiguous array.
 *
 * @param element_count The amount of elements in the array.
 * @param data_size     The size of each element in the array.
 *
 * @return The newly-initialized array, or NULL if memory allocation error.
 */
array array_init(const int element_count, const size_t data_size)
{
    struct _array *const init = malloc(sizeof(struct _array));
    if (init == NULL) {
        return NULL;
    }
    init->data_size = data_size;
    init->element_count = element_count;
    init->data = calloc((size_t) element_count, data_size);
    if (init->data == NULL) {
        free(init);
        return NULL;
    }
    return init;
}

/**
 * Gets the size of the array.
 *
 * @param me The array to check.
 *
 * @return The size of the array.
 */
int array_size(array me)
{
    return me->element_count;
}

/**
 * Copies the storage element of array to an array.
 *
 * @param arr The array to copy to.
 * @param me  The array to copy from.
 */
void array_to_array(void *const arr, array me)
{
    memcpy(arr, me->data, me->element_count * me->data_size);
}

/*
 * Determines if the input is illegal.
 */
static bool array_is_illegal_input(array me, const int index)
{
    return index < 0 || index >= me->element_count;
}

/**
 * Sets the data for a specified element in the array.
 *
 * @param me    The array to set data for.
 * @param index The location to set data at in the array.
 * @param data  The data to set at the location in the array.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int array_set(array me, const int index, void *const data)
{
    if (array_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    memcpy(me->data + index * me->data_size, data, me->data_size);
    return 0;
}

/**
 * Copies the element at index of the array to data.
 *
 * @param data  The data to copy to.
 * @param me    The array to copy from.
 * @param index The index to copy from in the array.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int array_get(void *const data, array me, const int index)
{
    if (array_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    memcpy(data, me->data + index * me->data_size, me->data_size);
    return 0;
}

/**
 * Frees the array memory.
 *
 * @param me The array to free from memory.
 *
 * @return NULL
 */
array array_destroy(array me)
{
    free(me->data);
    free(me);
    return NULL;
}
