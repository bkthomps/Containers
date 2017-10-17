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
#include "vector.h"

const int START_SPACE = 8;

struct _vector {
    size_t data_size;
    int offset;
    int space;
    void *storage;
};

/**
 * Initialize a vector.
 *
 * @param data_size The size of each element in the vector.
 *
 * @return The newly-initialized vector, or NULL if memory allocation error.
 */
vector vector_init(const size_t data_size) {
    struct _vector *const init = malloc(sizeof(struct _vector));
    if (init == NULL) {
        return NULL;
    }
    init->data_size = data_size;
    init->offset = 0;
    init->space = START_SPACE;
    init->storage = malloc(init->space * init->data_size);
    if (init->storage == NULL) {
        free(init);
        return NULL;
    }
    return init;
}

/**
 * Get the size being used by the vector.
 *
 * @param me The vector to check.
 *
 * @return The size being used by the vector.
 */
int vector_size(vector me) {
    return me->offset;
}

/**
 * Whether or not the vector is empty.
 *
 * @param me The vector to check.
 *
 * @return If the vector is empty.
 */
bool vector_is_empty(vector me) {
    return vector_size(me) == 0;
}

/**
 * Checks if the vector is big enough for the capacity specified.
 *
 * @param me       The vector to check.
 * @param capacity The capacity to check the vector for.
 *
 * @return True if big enough, else false.
 */
bool vector_ensure_capacity(vector me, const int capacity) {
    return capacity <= me->space;
}

/**
 * Sets the size to use for the vector buffer. The size is in units and not in
 * bytes. Each unit is of size specified from init. If the size to set is
 * smaller than the current size, the data is lost.
 *
 * @param me   The vector to set size.
 * @param size The size to set the vector to.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int vector_set_space(vector me, const int size) {
    me->space = size;
    if (me->space < me->offset) {
        me->offset = me->space;
    }
    void *const temp = realloc(me->storage, me->space * me->data_size);
    if (temp == NULL) {
        return -ENOMEM;
    }
    me->storage = temp;
    return 0;
}

/**
 * Sets the size of the vector buffer to the current size being used.
 *
 * @param me   The vector to trim.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int vector_trim_to_size(vector me) {
    return vector_set_space(me, me->offset);
}

/**
 * Copies the storage element of vector to an array.
 *
 * @param me    The vector to copy from.
 * @param array The array to copy to.
 */
void vector_to_array(void *array, vector me) {
    memcpy(array, me->storage, me->offset * me->data_size);
}

/**
 * Adds an element to the start of the vector.
 *
 * @param me   The vector to add to.
 * @param data The data to add to the vector.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int vector_add_first(vector me, void *const data) {
    return vector_add_at(me, 0, data);
}

/**
 * Adds an element to the location specified.
 *
 * @param me    The vector to add to.
 * @param index The location in the vector to add the data to.
 * @param data  The data to add to the vector.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 *         -EINVAL Invalid parameter.
 */
int vector_add_at(vector me, const int index, void *const data) {
    if (index < 0 || index > me->offset) {
        return -EINVAL;
    }
    const int full_space = me->space;
    if (me->offset + 1 >= me->space) {
        me->space *= 1.5;
        void *const temp = realloc(me->storage, me->space * me->data_size);
        if (temp == NULL) {
            return -ENOMEM;
        }
        me->storage = temp;
    }
    if (index != me->offset) {
        memmove(me->storage + (index + 1) * me->data_size,
                me->storage + index * me->data_size,
                (full_space - index) * me->data_size);
    }
    memcpy(me->storage + index * me->data_size, data, me->data_size);
    me->offset++;
    return 0;
}

/**
 * Adds an element to the end of the vector.
 *
 * @param me   The vector to add to.
 * @param data The data to add to the vector.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int vector_add_last(vector me, void *const data) {
    return vector_add_at(me, me->offset, data);
}

static bool isIllegalParameters(vector me, const int index) {
    return index < 0 || index >= me->offset || me->offset == 0;
}

/**
 * Removes the first element from the vector.
 *
 * @param me The vector to remove from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_remove_first(vector me) {
    return vector_remove_at(me, 0);
}

/**
 * Removes element based on its index.
 *
 * @param me    The vector to remove from.
 * @param index The location in the vector to remove the data from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_remove_at(vector me, const int index) {
    if (isIllegalParameters(me, index)) {
        return -EINVAL;
    }
    memmove(me->storage + index * me->data_size,
            me->storage + (index + 1) * me->data_size,
            (me->space - index) * me->data_size);
    me->offset--;
    return 0;
}

/**
 * Removes the last element from the vector.
 *
 * @param me The vector to remove from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_remove_last(vector me) {
    if (me->offset == 0) {
        return -EINVAL;
    }
    me->offset--;
    return 0;
}

/**
 * Sets the data for the first element in the vector.
 *
 * @param me The vector to set data for.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_set_first(vector me, void *const data) {
    return vector_set_at(me, 0, data);
}

/**
 *
 * @param me    The vector to set data for.
 * @param index The location to set data at in the vector.
 * @param data  The data to set at the location in the vector.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_set_at(vector me, const int index, void *const data) {
    if (isIllegalParameters(me, index)) {
        return -EINVAL;
    }
    memcpy(me->storage + index * me->data_size, data, me->data_size);
    return 0;
}

/**
 * Sets the data for the last element in the vector.
 *
 * @param me The vector to set data for.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_set_last(vector me, void *const data) {
    return vector_set_at(me, me->offset - 1, data);
}

/**
 * Copies the first element of the vector to data.
 *
 * @param data The data to copy to.
 * @param me   The vector to copy from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_get_first(void *const data, vector me) {
    return vector_get_at(data, me, 0);
}

/**
 * Copies the element at index of the vector to data.
 *
 * @param data  The data to copy to.
 * @param me    The vector to copy from.
 * @param index The index to copy from in the vector.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_get_at(void *const data, vector me, const int index) {
    if (isIllegalParameters(me, index)) {
        return -EINVAL;
    }
    memcpy(data, me->storage + index * me->data_size, me->data_size);
    return 0;
}

/**
 * Copies the last element of the vector to data.
 *
 * @param data The data to copy to.
 * @param me   The vector to copy from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid parameter.
 */
int vector_get_last(void *const data, vector me) {
    return vector_get_at(data, me, me->offset - 1);
}

/**
 * Clears the elements from the vector.
 *
 * @param me The vector to clear.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int vector_clear(vector me) {
    const int ret = vector_set_space(me, START_SPACE);
    me->offset = 0;
    return ret;
}

/**
 * Frees the vector memory.
 *
 * @param me The vector to free from memory.
 *
 * @return NULL
 */
vector vector_destroy(vector me) {
    free(me->storage);
    me->storage = NULL;
    free(me);
    return NULL;
}
