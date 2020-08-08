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
#include "include/deque.h"

static const size_t BLOCK_SIZE = 8; /* TODO: change to 1024 after testing */
static const size_t INITIAL_BLOCK_COUNT = 1;
static const double RESIZE_RATIO = 1.5;

struct internal_deque {
    size_t data_size;
    size_t start_index;
    size_t end_index;
    size_t block_count;
    char **data;
};

/**
 * Initializes a deque.
 *
 * @param data_size the size of each element in the deque; must be positive
 *
 * @return the newly-initialized deque, or NULL if it was not successfully
 *         initialized due to either invalid input arguments or memory
 *         allocation error
 */
deque deque_init(const size_t data_size)
{
    struct internal_deque *init;
    char *block;
    if (data_size == 0) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_deque));
    if (!init) {
        return NULL;
    }
    init->data_size = data_size;
    init->start_index = BLOCK_SIZE * INITIAL_BLOCK_COUNT / 2;
    init->end_index = init->start_index;
    init->block_count = INITIAL_BLOCK_COUNT;
    init->data = calloc(init->block_count, sizeof(char *));
    if (!init->data) {
        free(init);
        return NULL;
    }
    block = malloc(BLOCK_SIZE * init->data_size);
    if (!block) {
        free(init->data);
        free(init);
        return NULL;
    }
    memcpy(init->data + init->start_index / BLOCK_SIZE, &block, sizeof(char *));
    return init;
}

/**
 * Determines the size of the deque. The size is the number of data spaces being
 * used. The size starts at zero, and every time an element is added, it
 * increases by one.
 *
 * @param me the deque to check the size of
 *
 * @return the size of the deque
 */
size_t deque_size(deque me)
{
    return me->end_index - me->start_index;
}

/**
 * Determines if the deque is empty. It is empty if it has no elements.
 *
 * @param me the deque to check if empty
 *
 * @return 1 if the deque is empty, otherwise 0
 */
int deque_is_empty(deque me)
{
    return deque_size(me) == 0;
}

/**
 * Trims the deque so that it does not use memory which does not need to be
 * used.
 *
 * @param me the deque to trim
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int deque_trim(deque me)
{
    const size_t start_block_index = me->start_index / BLOCK_SIZE;
    const size_t end_block_index = deque_is_empty(me) ? start_block_index :
                                   (me->end_index - 1) / BLOCK_SIZE;
    const size_t updated_block_count = end_block_index - start_block_index + 1;
    char **updated_data = malloc(updated_block_count * sizeof(char *));
    if (!updated_data) {
        return -ENOMEM;
    }
    memcpy(updated_data, me->data + start_block_index,
           updated_block_count * sizeof(char *));
    free(me->data);
    me->start_index -= start_block_index * BLOCK_SIZE;
    me->end_index -= start_block_index * BLOCK_SIZE;
    me->block_count = updated_block_count;
    me->data = updated_data;
    return 0;
}

/**
 * Copies the deque to an array. Since it is a copy, the array may be modified
 * without causing side effects to the deque data structure. Memory is not
 * allocated, thus the array being used for the copy must be allocated before
 * this function is called. The size of the deque should be queried prior to
 * calling this function, which also serves as the size of the newly-copied
 * array.
 *
 * @param arr the initialized array to copy the deque to
 * @param me  the deque to copy to the array
 */
void deque_copy_to_array(void *const arr, deque me)
{
    char *block;
    size_t i;
    size_t size_offset;
    const size_t start_block_index = me->start_index / BLOCK_SIZE;
    const size_t start_inner_index = me->start_index % BLOCK_SIZE;
    const size_t end_block_index = (me->end_index - 1) / BLOCK_SIZE;
    const size_t end_inner_index = (me->end_index - 1) % BLOCK_SIZE;
    const size_t first_block_length = BLOCK_SIZE < deque_size(me)
                                      ? BLOCK_SIZE - start_inner_index
                                      : deque_size(me);
    if (deque_is_empty(me)) {
        return;
    }
    size_offset = first_block_length * me->data_size;
    memcpy(&block, me->data + start_block_index, sizeof(char *));
    memcpy(arr, block + start_inner_index * me->data_size, size_offset);
    for (i = start_block_index + 1; i < end_block_index; i++) {
        memcpy(&block, me->data + i, sizeof(char *));
        memcpy((char *) arr + size_offset, block, BLOCK_SIZE * me->data_size);
        size_offset += BLOCK_SIZE * me->data_size;
    }
    if (end_block_index != start_block_index) {
        memcpy(&block, me->data + end_block_index, sizeof(char *));
        memcpy((char *) arr + size_offset, block,
               (end_inner_index + 1) * me->data_size);
    }
}

/**
 * Adds an element to the front of the deque. The pointer to the data being
 * passed in should point to the data type which this deque holds. For example,
 * if this deque holds integers, the data pointer should be a pointer to an
 * integer. Since the data is being copied, the pointer only has to be valid
 * when this function is called.
 *
 * @param me   the deque to add an element to
 * @param data the element to add
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int deque_push_front(deque me, void *const data)
{
    if (me->start_index == 0) {
        const size_t updated_block_count =
                (size_t) (me->block_count * RESIZE_RATIO) + 1;
        const size_t added_blocks = updated_block_count - me->block_count;
        char **temp = realloc(me->data, updated_block_count * sizeof(char *));
        if (!temp) {
            return -ENOMEM;
        }
        memmove(temp + added_blocks, temp, me->block_count * sizeof(char *));
        memset(temp, 0, added_blocks * sizeof(char *));
        me->data = temp;
        me->block_count += added_blocks;
        me->start_index += added_blocks * BLOCK_SIZE;
        me->end_index += added_blocks * BLOCK_SIZE;
    }
    if (me->start_index % BLOCK_SIZE == 0) {
        char *block;
        const size_t previous_block_index = me->start_index / BLOCK_SIZE - 1;
        memcpy(&block, me->data + previous_block_index, sizeof(char *));
        if (block) {
            goto copy_element;
        }
        block = malloc(BLOCK_SIZE * me->data_size);
        if (!block) {
            return -ENOMEM;
        }
        memcpy(me->data + previous_block_index, &block, sizeof(char *));
    }
    copy_element:
    me->start_index--;
    {
        char *block;
        const size_t block_index = me->start_index / BLOCK_SIZE;
        const size_t inner_index = me->start_index % BLOCK_SIZE;
        memcpy(&block, me->data + block_index, sizeof(char *));
        memcpy(block + inner_index * me->data_size, data, me->data_size);
    }
    return 0;
}

/**
 * Adds an element to the back of the deque. The pointer to the data being
 * passed in should point to the data type which this deque holds. For example,
 * if this deque holds integers, the data pointer should be a pointer to an
 * integer. Since the data is being copied, the pointer only has to be valid
 * when this function is called.
 *
 * @param me   the deque to add an element to
 * @param data the element to add
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int deque_push_back(deque me, void *const data)
{
    if (me->end_index == me->block_count * BLOCK_SIZE) {
        const size_t updated_block_count =
                (size_t) (me->block_count * RESIZE_RATIO) + 1;
        const size_t added_blocks = updated_block_count - me->block_count;
        char **temp = realloc(me->data, updated_block_count * sizeof(char *));
        if (!temp) {
            return -ENOMEM;
        }
        memset(temp + me->block_count, 0, added_blocks * sizeof(char *));
        me->data = temp;
        me->block_count += added_blocks;
    }
    if (me->end_index % BLOCK_SIZE == 0) {
        char *block;
        const size_t tentative_block_index = me->end_index / BLOCK_SIZE;
        memcpy(&block, me->data + tentative_block_index, sizeof(char *));
        if (block) {
            goto copy_element;
        }
        block = malloc(BLOCK_SIZE * me->data_size);
        if (!block) {
            return -ENOMEM;
        }
        memcpy(me->data + tentative_block_index, &block, sizeof(char *));
    }
    copy_element:
    {
        char *block;
        const size_t block_index = me->end_index / BLOCK_SIZE;
        const size_t inner_index = me->end_index % BLOCK_SIZE;
        memcpy(&block, me->data + block_index, sizeof(char *));
        memcpy(block + inner_index * me->data_size, data, me->data_size);
    }
    me->end_index++;
    return 0;
}

/**
 * Removes the front element from the deque and copies it to a data value. The
 * pointer to the data being obtained should point to the data type which this
 * deque holds. For example, if this deque holds integers, the data pointer
 * should be a pointer to an integer. Since this data is being copied from the
 * array to the data pointer, the pointer only has to be valid when this
 * function is called.
 *
 * @param data the value to copy to
 * @param me   the deque to remove from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int deque_pop_front(void *const data, deque me)
{
    char *block;
    const size_t block_index = me->start_index / BLOCK_SIZE;
    const size_t inner_index = me->start_index % BLOCK_SIZE;
    if (deque_is_empty(me)) {
        return -EINVAL;
    }
    memcpy(&block, me->data + block_index, sizeof(char *));
    memcpy(data, block + inner_index * me->data_size, me->data_size);
    me->start_index++;
    return 0;
}

/**
 * Removes the back element from the deque and copies it to a data value. The
 * pointer to the data being obtained should point to the data type which this
 * deque holds. For example, if this deque holds integers, the data pointer
 * should be a pointer to an integer. Since this data is being copied from the
 * array to the data pointer, the pointer only has to be valid when this
 * function is called.
 *
 * @param data the value to copy to
 * @param me   the deque to remove from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int deque_pop_back(void *const data, deque me)
{
    char *block;
    const size_t block_index = (me->end_index - 1) / BLOCK_SIZE;
    const size_t inner_index = (me->end_index - 1) % BLOCK_SIZE;
    if (deque_is_empty(me)) {
        return -EINVAL;
    }
    memcpy(&block, me->data + block_index, sizeof(char *));
    memcpy(data, block + inner_index * me->data_size, me->data_size);
    me->end_index--;
    return 0;
}

/**
 * Sets the first value of the deque. The pointer to the data being passed in
 * should point to the data type which this deque holds. For example, if this
 * deque holds integers, the data pointer should be a pointer to an integer.
 * Since the data is being copied, the pointer only has to be valid when this
 * function is called.
 *
 * @param me   the deque to set the value of
 * @param data the data to set
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int deque_set_first(deque me, void *const data)
{
    return deque_set_at(me, 0, data);
}

/**
 * Sets the value of the deque at the specified index. The pointer to the data
 * being passed in should point to the data type which this deque holds. For
 * example, if this deque holds integers, the data pointer should be a pointer
 * to an integer. Since the data is being copied, the pointer only has to be
 * valid when this function is called.
 *
 * @param me    the deque to set the value of
 * @param index the index to set at
 * @param data  the data to set
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int deque_set_at(deque me, size_t index, void *const data)
{
    char *block;
    const size_t block_index = (index + me->start_index) / BLOCK_SIZE;
    const size_t inner_index = (index + me->start_index) % BLOCK_SIZE;
    if (index >= deque_size(me)) {
        return -EINVAL;
    }
    memcpy(&block, me->data + block_index, sizeof(char *));
    memcpy(block + inner_index * me->data_size, data, me->data_size);
    return 0;
}

/**
 * Sets the last value of the deque. The pointer to the data being passed in
 * should point to the data type which this deque holds. For example, if this
 * deque holds integers, the data pointer should be a pointer to an integer.
 * Since the data is being copied, the pointer only has to be valid when this
 * function is called.
 *
 * @param me   the deque to set the value of
 * @param data the data to set
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int deque_set_last(deque me, void *const data)
{
    return deque_set_at(me, deque_size(me) - 1, data);
}

/**
 * Gets the first value of the deque. The pointer to the data being obtained
 * should point to the data type which this deque holds. For example, if this
 * deque holds integers, the data pointer should be a pointer to an integer.
 * Since this data is being copied from the array to the data pointer, the
 * pointer only has to be valid when this function is called.
 *
 * @param data the data to set
 * @param me   the deque to set the value of
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int deque_get_first(void *const data, deque me)
{
    return deque_get_at(data, me, 0);
}

/**
 * Gets the value of the deque at the specified index. The pointer to the data
 * being obtained should point to the data type which this deque holds. For
 * example, if this deque holds integers, the data pointer should be a pointer
 * to an integer. Since this data is being copied from the array to the data
 * pointer, the pointer only has to be valid when this function is called.
 *
 * @param data  the data to set
 * @param me    the deque to set the value of
 * @param index the index to set at
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int deque_get_at(void *const data, deque me, size_t index)
{
    char *block;
    const size_t block_index = (index + me->start_index) / BLOCK_SIZE;
    const size_t inner_index = (index + me->start_index) % BLOCK_SIZE;
    if (index >= deque_size(me)) {
        return -EINVAL;
    }
    memcpy(&block, me->data + block_index, sizeof(char *));
    memcpy(data, block + inner_index * me->data_size, me->data_size);
    return 0;
}

/**
 * Gets the last value of the deque. The pointer to the data being obtained
 * should point to the data type which this deque holds. For example, if this
 * deque holds integers, the data pointer should be a pointer to an integer.
 * Since this data is being copied from the array to the data pointer, the
 * pointer only has to be valid when this function is called.
 *
 * @param data the data to set
 * @param me   the deque to set the value of
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int deque_get_last(void *const data, deque me)
{
    return deque_get_at(data, me, deque_size(me) - 1);
}

/**
 * Clears the deque and sets it to the original state from initialization.
 *
 * @param me the deque to clear
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int deque_clear(deque me)
{
    size_t i;
    char *updated_block;
    char **updated_data = calloc(INITIAL_BLOCK_COUNT, sizeof(char *));
    if (!updated_data) {
        return -ENOMEM;
    }
    updated_block = malloc(BLOCK_SIZE * me->data_size);
    if (!updated_block) {
        free(updated_data);
        return -ENOMEM;
    }
    for (i = 0; i < me->block_count; i++) {
        char *block;
        memcpy(&block, me->data + i, sizeof(char *));
        free(block);
    }
    free(me->data);
    me->start_index = BLOCK_SIZE * INITIAL_BLOCK_COUNT / 2;
    me->end_index = me->start_index;
    me->block_count = INITIAL_BLOCK_COUNT;
    me->data = updated_data;
    memcpy(me->data + me->start_index / BLOCK_SIZE, &updated_block,
           sizeof(char *));
    return 0;
}

/**
 * Destroys the deque. Performing further operations after calling this function
 * results in undefined behavior.
 *
 * @param me the deque to destroy
 *
 * @return NULL
 */
deque deque_destroy(deque me)
{
    size_t i;
    for (i = 0; i < me->block_count; i++) {
        char *block;
        memcpy(&block, me->data + i, sizeof(char *));
        free(block);
    }
    free(me->data);
    free(me);
    return NULL;
}
