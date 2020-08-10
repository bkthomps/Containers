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
#include "include/forward_list.h"

struct internal_forward_list {
    size_t bytes_per_item;
    size_t item_count;
    char *head;
    char *tail;
};

const size_t ptr_size = sizeof(char *);
const size_t node_next_ptr_offset = 0;
const size_t node_data_ptr_offset = sizeof(char *);

/**
 * Initializes a singly-linked list.
 *
 * @param data_size the size of data to store; must be positive
 *
 * @return the newly-initialized singly-linked list, or NULL if it was not
 *         successfully initialized due to either invalid input arguments or
 *         memory allocation error
 */
forward_list forward_list_init(const size_t data_size)
{
    struct internal_forward_list *init;
    if (data_size == 0) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_forward_list));
    if (!init) {
        return NULL;
    }
    init->bytes_per_item = data_size;
    init->item_count = 0;
    init->head = NULL;
    init->tail = NULL;
    return init;
}

/**
 * Gets the number of elements in the singly-linked list.
 *
 * @param me the singly-linked list to check
 *
 * @return the number of elements
 */
size_t forward_list_size(forward_list me)
{
    return me->item_count;
}

/**
 * Determines if the singly-linked list is empty.
 *
 * @param me the singly-linked list to check
 *
 * @return 1 if the singly-linked list is empty, otherwise 0
 */
int forward_list_is_empty(forward_list me)
{
    return forward_list_size(me) == 0;
}

/**
 * Copies the nodes of the singly-linked list to an array. Since it is a copy,
 * the array may be modified without causing side effects to the singly-linked
 * list data structure. Memory is not allocated, thus the array being used for
 * the copy must be allocated before this function is called. The size of the
 * singly-linked list should be queried prior to calling this function, which
 * also serves as the size of the newly-copied array.
 *
 * @param arr the initialized array to copy the singly-linked list to
 * @param me  the singly-linked list to copy to the array
 */
void forward_list_copy_to_array(void *const arr, forward_list me)
{
    char *traverse = me->head;
    size_t offset = 0;
    while (traverse) {
        memcpy((char *) arr + offset, traverse + node_data_ptr_offset,
               me->bytes_per_item);
        memcpy(&traverse, traverse + node_next_ptr_offset, ptr_size);
        offset += me->bytes_per_item;
    }
}

/*
 * Gets the node at the specified index.
 */
static char *forward_list_get_node_at(forward_list me, const size_t index)
{
    char *traverse = me->head;
    char *traverse_next;
    size_t i;
    if (me->tail && index == me->item_count - 1) {
        return me->tail;
    }
    for (i = 0; i < index; i++) {
        memcpy(&traverse, traverse + node_next_ptr_offset, ptr_size);
    }
    memcpy(&traverse_next, traverse + node_next_ptr_offset, ptr_size);
    if (!traverse_next) {
        me->tail = traverse;
    }
    return traverse;
}

/**
 * Adds data at the first index in the singly-linked list. The pointer to the
 * data being passed in should point to the data type which this singly-linked
 * list holds. For example, if this singly-linked list holds integers, the data
 * pointer should be a pointer to an integer. Since the data is being copied,
 * the pointer only has to be valid when this function is called.
 *
 * @param me   the singly-linked list to add data to
 * @param data the data to add to the singly-linked list
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int forward_list_add_first(forward_list me, void *const data)
{
    return forward_list_add_at(me, 0, data);
}

/**
 * Adds data at a specified index in the singly-linked list. The pointer to the
 * data being passed in should point to the data type which this singly-linked
 * list holds. For example, if this singly-linked list holds integers, the data
 * pointer should be a pointer to an integer. Since the data is being copied,
 * the pointer only has to be valid when this function is called.
 *
 * @param me    the singly-linked list to add data to
 * @param index the index to add the data at
 * @param data  the data to add to the singly-linked list
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 * @return -EINVAL if invalid argument
 */
int forward_list_add_at(forward_list me, const size_t index, void *const data)
{
    char *node;
    if (index > me->item_count) {
        return -EINVAL;
    }
    node = malloc(ptr_size + me->bytes_per_item);
    if (!node) {
        return -ENOMEM;
    }
    memcpy(node + node_data_ptr_offset, data, me->bytes_per_item);
    if (index == 0) {
        memcpy(node + node_next_ptr_offset, &me->head, ptr_size);
        me->head = node;
    } else {
        char *node_next;
        char *traverse = forward_list_get_node_at(me, index - 1);
        memcpy(node + node_next_ptr_offset, traverse + node_next_ptr_offset,
               ptr_size);
        memcpy(traverse + node_next_ptr_offset, &node, ptr_size);
        memcpy(&node_next, node + node_next_ptr_offset, ptr_size);
        if (!node_next) {
            me->tail = node;
        }
    }
    me->item_count++;
    return 0;
}

/**
 * Adds data at the last index in the singly-linked list. The pointer to the
 * data being passed in should point to the data type which this singly-linked
 * list holds. For example, if this singly-linked list holds integers, the data
 * pointer should be a pointer to an integer. Since the data is being copied,
 * the pointer only has to be valid when this function is called.
 *
 * @param me   the singly-linked list to add data to
 * @param data the data to add to the singly-linked list
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int forward_list_add_last(forward_list me, void *const data)
{
    return forward_list_add_at(me, me->item_count, data);
}

/**
 * Removes the first piece of data from the singly-linked list.
 *
 * @param me the singly-linked list to remove data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_remove_first(forward_list me)
{
    return forward_list_remove_at(me, 0);
}

/**
 * Removes data from the singly-linked list at the specified index.
 *
 * @param me    the singly-linked list to remove data from
 * @param index the index to remove from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_remove_at(forward_list me, const size_t index)
{
    if (index >= me->item_count) {
        return -EINVAL;
    }
    if (index == 0) {
        char *temp = me->head;
        memcpy(&me->head, temp + node_next_ptr_offset, ptr_size);
        if (!me->head) {
            me->tail = NULL;
        }
        free(temp);
    } else {
        char *traverse = forward_list_get_node_at(me, index - 1);
        char *backup;
        char *backup_next;
        memcpy(&backup, traverse + node_next_ptr_offset, ptr_size);
        memcpy(traverse + node_next_ptr_offset, backup + node_next_ptr_offset,
               ptr_size);
        memcpy(&backup_next, backup + node_next_ptr_offset, ptr_size);
        if (!backup_next) {
            me->tail = NULL;
        }
        free(backup);
    }
    me->item_count--;
    return 0;
}

/**
 * Removes the last piece of data from the singly-linked list.
 *
 * @param me the singly-linked list to remove data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_remove_last(forward_list me)
{
    return forward_list_remove_at(me, me->item_count - 1);
}

/**
 * Sets the data at the first index in the singly-linked list. The pointer to
 * the data being passed in should point to the data type which this singly-
 * linked list holds. For example, if this singly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since the data is being
 * copied, the pointer only has to be valid when this function is called.
 *
 * @param me   the singly-linked list to set data for
 * @param data the data to set in the singly-linked list
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_set_first(forward_list me, void *const data)
{
    return forward_list_set_at(me, 0, data);
}

/**
 * Sets the data at the specified index in the singly-linked list. The pointer
 * to the data being passed in should point to the data type which this singly-
 * linked list holds. For example, if this singly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since the data is being
 * copied, the pointer only has to be valid when this function is called.
 *
 * @param me    the singly-linked list to set data for
 * @param index the index to set data in the singly-linked list
 * @param data  the data to set in the singly-linked list
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_set_at(forward_list me, const size_t index, void *const data)
{
    char *traverse;
    if (index >= me->item_count) {
        return -EINVAL;
    }
    traverse = forward_list_get_node_at(me, index);
    memcpy(traverse + node_data_ptr_offset, data, me->bytes_per_item);
    return 0;
}

/**
 * Sets the data at the last index in the singly-linked list. The pointer to
 * the data being passed in should point to the data type which this singly-
 * linked list holds. For example, if this singly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since the data is being
 * copied, the pointer only has to be valid when this function is called.
 *
 * @param me   the singly-linked list to set data for
 * @param data the data to set in the singly-linked list
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_set_last(forward_list me, void *const data)
{
    return forward_list_set_at(me, me->item_count - 1, data);
}

/**
 * Gets the data at the first index in the singly-linked list. The pointer to
 * the data being obtained should point to the data type which this singly-
 * linked list holds. For example, if this singly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since this data is being
 * copied from the array to the data pointer, the pointer only has to be valid
 * when this function is called.
 *
 * @param data the data to get
 * @param me   the singly-linked list to get data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_get_first(void *const data, forward_list me)
{
    return forward_list_get_at(data, me, 0);
}

/**
 * Gets the data at the specified index in the singly-linked list. The pointer
 * to the data being obtained should point to the data type which this singly-
 * linked list holds. For example, if this singly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since this data is being
 * copied from the array to the data pointer, the pointer only has to be valid
 * when this function is called.
 *
 * @param data  the data to get
 * @param me    the singly-linked list to get data from
 * @param index the index to get data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_get_at(void *const data, forward_list me, const size_t index)
{
    char *traverse;
    if (index >= me->item_count) {
        return -EINVAL;
    }
    traverse = forward_list_get_node_at(me, index);
    memcpy(data, traverse + node_data_ptr_offset, me->bytes_per_item);
    return 0;
}

/**
 * Gets the data at the last index in the singly-linked list. The pointer to
 * the data being obtained should point to the data type which this singly-
 * linked list holds. For example, if this singly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since this data is being
 * copied from the array to the data pointer, the pointer only has to be valid
 * when this function is called.
 *
 * @param data the data to get
 * @param me   the singly-linked list to get data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_get_last(void *const data, forward_list me)
{
    return forward_list_get_at(data, me, me->item_count - 1);
}

/**
 * Clears all elements from the singly-linked list.
 *
 * @param me the singly-linked list to clear
 */
void forward_list_clear(forward_list me)
{
    char *traverse = me->head;
    while (traverse) {
        char *temp = traverse;
        memcpy(&traverse, traverse + node_next_ptr_offset, ptr_size);
        free(temp);
    }
    me->head = NULL;
    me->tail = NULL;
    me->item_count = 0;
}

/**
 * Destroys the singly-linked list. Performing further operations after calling
 * this function results in undefined behavior.
 *
 * @param me the singly-linked list to destroy
 *
 * @return NULL
 */
forward_list forward_list_destroy(forward_list me)
{
    forward_list_clear(me);
    free(me);
    return NULL;
}
