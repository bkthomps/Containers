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

#include <string.h>
#include <errno.h>
#include "include/array.h"

/**
 * Initializes an array. If the multiplication of the element count and the
 * data size overflows, it is undefined behavior.
 *
 * @param element_count the number of elements in the array; must not be
 *                      negative
 * @param data_size     the size of each element in the array; must be positive
 *
 * @return the newly-initialized array, or NULL if it was not successfully
 *         initialized due to either invalid input arguments or memory
 *         allocation error
 */
array array_init(const size_t element_count, const size_t data_size)
{
    char *init;
    if (data_size == 0) {
        return NULL;
    }
    init = malloc(2 * sizeof(size_t) + element_count * data_size);
    if (!init) {
        return NULL;
    }
    memcpy(init, &element_count, sizeof(size_t));
    memcpy(init + sizeof(size_t), &data_size, sizeof(size_t));
    memset(init + 2 * sizeof(size_t), 0, element_count * data_size);
    return init;
}

/**
 * Gets the size of the array.
 *
 * @param me the array to check
 *
 * @return the size of the array
 */
size_t array_size(array me)
{
    size_t size;
    memcpy(&size, me, sizeof(size_t));
    return size;
}

/**
 * Copies the array to a raw array. Since it is a copy, the raw array may be
 * modified without causing side effects to the array data structure. Memory
 * is not allocated, thus the raw array being used for the copy must be
 * allocated before this function is called. The size of the array should be
 * queried prior to calling this function, which also serves as the size of the
 * newly-copied raw array.
 *
 * @param arr the initialized raw array to copy the array to
 * @param me  the array to copy to the raw array
 */
void array_copy_to_array(void *const arr, array me)
{
    size_t element_count;
    size_t data_size;
    memcpy(&element_count, me, sizeof(size_t));
    memcpy(&data_size, me + sizeof(size_t), sizeof(size_t));
    memcpy(arr, me + 2 * sizeof(size_t), element_count * data_size);
}

/**
 * Gets the storage element of the array structure. The storage element is
 * contiguous in memory. The data pointer should be assigned to the correct
 * array type. For example, if the array holds integers, the data pointer should
 * be assigned to a raw integer array. The size of the array should be obtained
 * prior to calling this function, which also serves as the size of the queried
 * raw array. This pointer is not a copy, thus any modification to the data will
 * cause the array structure data to be modified. Operations using the array
 * functions may invalidate this pointer. The array owns this memory, thus it
 * should not be freed.
 *
 * @param me the array to get the storage element from
 *
 * @return the storage element of the array
 */
void *array_get_data(array me)
{
    size_t element_count;
    memcpy(&element_count, me, sizeof(size_t));
    if (element_count == 0) {
        return NULL;
    }
    return me + 2 * sizeof(size_t);
}

/**
 * Sets the data for a specified element in the array. The pointer to the data
 * being passed in should point to the data type which this array holds. For
 * example, if this array holds integers, the data pointer should be a pointer
 * to an integer. Since the data is being copied, the pointer only has to be
 * valid when this function is called.
 *
 * @param me    the array to set data for
 * @param index the location to set data at in the array
 * @param data  the data to set at the location in the array
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int array_set(array me, const size_t index, void *const data)
{
    size_t element_count;
    size_t data_size;
    memcpy(&element_count, me, sizeof(size_t));
    if (index >= element_count) {
        return -EINVAL;
    }
    memcpy(&data_size, me + sizeof(size_t), sizeof(size_t));
    memcpy(me + 2 * sizeof(size_t) + index * data_size, data, data_size);
    return 0;
}

/**
 * Copies the element at an index of the array to data. The pointer to the data
 * being obtained should point to the data type which this array holds. For
 * example, if this array holds integers, the data pointer should be a pointer
 * to an integer. Since this data is being copied from the array to the data
 * pointer, the pointer only has to be valid when this function is called.
 *
 *
 * @param data  the data to copy to
 * @param me    the array to copy from
 * @param index the index to copy from in the array
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int array_get(void *const data, array me, const size_t index)
{
    size_t element_count;
    size_t data_size;
    memcpy(&element_count, me, sizeof(size_t));
    if (index >= element_count) {
        return -EINVAL;
    }
    memcpy(&data_size, me + sizeof(size_t), sizeof(size_t));
    memcpy(data, me + 2 * sizeof(size_t) + index * data_size, data_size);
    return 0;
}

/**
 * Frees the array memory. Performing further operations after calling this
 * function results in undefined behavior.
 *
 * @param me the array to free from memory
 *
 * @return NULL
 */
array array_destroy(array me)
{
    free(me);
    return NULL;
}
