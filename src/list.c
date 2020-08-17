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
#include "include/list.h"

struct internal_list {
    size_t bytes_per_item;
    size_t item_count;
    char *head;
    char *tail;
};

static const size_t ptr_size = sizeof(char *);
static const size_t node_next_ptr_offset = 0;
static const size_t node_prev_ptr_offset = sizeof(char *);
static const size_t node_data_ptr_offset = 2 * sizeof(char *);

/**
 * Initializes a doubly-linked list.
 *
 * @param data_size the size of data to store; must be positive
 *
 * @return the newly-initialized doubly-linked list, or NULL if it was not
 *         successfully initialized due to either invalid input arguments or
 *         memory allocation error
 */
list list_init(const size_t data_size)
{
    struct internal_list *init;
    if (data_size == 0) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_list));
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
 * Gets the number of elements in the doubly-linked list.
 *
 * @param me the doubly-linked list to check
 *
 * @return the number of elements
 */
size_t list_size(list me)
{
    return me->item_count;
}

/**
 * Determines if the doubly-linked list is empty.
 *
 * @param me the doubly-linked list to check
 *
 * @return 1 if the list is empty, otherwise 0
 */
int list_is_empty(list me)
{
    return list_size(me) == 0;
}

/**
 * Copies the nodes of the doubly-linked list to an array. Since it is a copy,
 * the array may be modified without causing side effects to the doubly-linked
 * list data structure. Memory is not allocated, thus the array being used for
 * the copy must be allocated before this function is called. The size of the
 * doubly-linked list should be queried prior to calling this function, which
 * also serves as the size of the newly-copied array.
 *
 * @param arr the initialized array to copy the doubly-linked list to
 * @param me  the doubly-linked list to copy to the array
 */
void list_copy_to_array(void *const arr, list me)
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
 * Gets the node at index starting from the head.
 */
static char *list_get_node_from_head(list me, const size_t index)
{
    char *traverse = me->head;
    size_t i;
    for (i = 0; i < index; i++) {
        memcpy(&traverse, traverse + node_next_ptr_offset, ptr_size);
    }
    return traverse;
}

/*
 * Gets the node at index starting from the tail.
 */
static char *list_get_node_from_tail(list me, const size_t index)
{
    char *traverse = me->tail;
    size_t i;
    for (i = me->item_count - 1; i > index; i--) {
        memcpy(&traverse, traverse + node_prev_ptr_offset, ptr_size);
    }
    return traverse;
}

/*
 * Gets the node at the specified index.
 */
static char *list_get_node_at(list me, const size_t index)
{
    if (index < me->item_count / 2) {
        return list_get_node_from_head(me, index);
    }
    return list_get_node_from_tail(me, index);
}

/**
 * Adds data at the first index in the doubly-linked list. The pointer to the
 * data being passed in should point to the data type which this doubly-linked
 * list holds. For example, if this doubly-linked list holds integers, the data
 * pointer should be a pointer to an integer. Since the data is being copied,
 * the pointer only has to be valid when this function is called.
 *
 * @param me   the doubly-linked list to add data to
 * @param data the data to add to the doubly-linked list
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int list_add_first(list me, void *const data)
{
    return list_add_at(me, 0, data);
}

/**
 * Adds data at a specified index in the doubly-linked list. The pointer to the
 * data being passed in should point to the data type which this doubly-linked
 * list holds. For example, if this doubly-linked list holds integers, the data
 * pointer should be a pointer to an integer. Since the data is being copied,
 * the pointer only has to be valid when this function is called.
 *
 * @param me    the doubly-linked list to add data to
 * @param index the index to add the data at
 * @param data  the data to add to the doubly-linked list
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 * @return -EINVAL if invalid argument
 */
int list_add_at(list me, const size_t index, void *const data)
{
    char *node;
    if (index > me->item_count) {
        return -EINVAL;
    }
    /* TODO: check this for "overflow" */
    node = malloc(2 * ptr_size + me->bytes_per_item);
    if (!node) {
        return -ENOMEM;
    }
    memcpy(node + node_data_ptr_offset, data, me->bytes_per_item);
    if (!me->head) {
        /* Relies on next and prev being the first two blocks. */
        memset(node, 0, 2 * ptr_size);
        me->head = node;
        me->tail = node;
    } else if (index == 0) {
        char *traverse = me->head;
        memcpy(traverse + node_prev_ptr_offset, &node, ptr_size);
        memcpy(node + node_next_ptr_offset, &traverse, ptr_size);
        memset(node + node_prev_ptr_offset, 0, ptr_size);
        me->head = node;
    } else if (index == me->item_count) {
        char *traverse = me->tail;
        memcpy(traverse + node_next_ptr_offset, &node, ptr_size);
        memset(node + node_next_ptr_offset, 0, ptr_size);
        memcpy(node + node_prev_ptr_offset, &traverse, ptr_size);
        me->tail = node;
    } else {
        char *traverse = list_get_node_at(me, index);
        char *traverse_prev;
        memcpy(&traverse_prev, traverse + node_prev_ptr_offset, ptr_size);
        memcpy(node + node_prev_ptr_offset, &traverse_prev, ptr_size);
        memcpy(node + node_next_ptr_offset, &traverse, ptr_size);
        memcpy(traverse_prev + node_next_ptr_offset, &node, ptr_size);
        memcpy(traverse + node_prev_ptr_offset, &node, ptr_size);
    }
    me->item_count++;
    return 0;
}

/**
 * Adds data at the last index in the doubly-linked list. The pointer to the
 * data being passed in should point to the data type which this doubly-linked
 * list holds. For example, if this doubly-linked list holds integers, the data
 * pointer should be a pointer to an integer. Since the data is being copied,
 * the pointer only has to be valid when this function is called.
 *
 * @param me   the doubly-linked list to add data to
 * @param data the data to add to the doubly-linked list
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int list_add_last(list me, void *const data)
{
    return list_add_at(me, me->item_count, data);
}

/**
 * Removes the first piece of data from the doubly-linked list.
 *
 * @param me the doubly-linked list to remove data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_remove_first(list me)
{
    return list_remove_at(me, 0);
}

/**
 * Removes data from the doubly-linked list at the specified index.
 *
 * @param me    the doubly-linked list to remove data from
 * @param index the index to remove from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_remove_at(list me, const size_t index)
{
    char *traverse;
    if (index >= me->item_count) {
        return -EINVAL;
    }
    traverse = list_get_node_at(me, index);
    if (me->item_count == 1) {
        me->head = NULL;
        me->tail = NULL;
    } else if (index == 0) {
        char *traverse_next;
        memcpy(&traverse_next, traverse + node_next_ptr_offset, ptr_size);
        memset(traverse_next + node_prev_ptr_offset, 0, ptr_size);
        me->head = traverse_next;
    } else if (index == me->item_count - 1) {
        char *traverse_prev;
        memcpy(&traverse_prev, traverse + node_prev_ptr_offset, ptr_size);
        memset(traverse_prev + node_next_ptr_offset, 0, ptr_size);
        me->tail = traverse_prev;
    } else {
        char *traverse_next;
        char *traverse_prev;
        memcpy(&traverse_next, traverse + node_next_ptr_offset, ptr_size);
        memcpy(&traverse_prev, traverse + node_prev_ptr_offset, ptr_size);
        memcpy(traverse_next + node_prev_ptr_offset, &traverse_prev, ptr_size);
        memcpy(traverse_prev + node_next_ptr_offset, &traverse_next, ptr_size);
    }
    free(traverse);
    me->item_count--;
    return 0;
}

/**
 * Removes the last piece of data from the doubly-linked list.
 *
 * @param me the doubly-linked list to remove data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_remove_last(list me)
{
    return list_remove_at(me, me->item_count - 1);
}

/**
 * Sets the data at the first index in the doubly-linked list. The pointer to
 * the data being passed in should point to the data type which this doubly-
 * linked list holds. For example, if this doubly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since the data is being
 * copied, the pointer only has to be valid when this function is called.
 *
 * @param me   the doubly-linked list to set data for
 * @param data the data to set in the doubly-linked list
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_set_first(list me, void *const data)
{
    return list_set_at(me, 0, data);
}

/**
 * Sets the data at the specified index in the doubly-linked list. The pointer
 * to the data being passed in should point to the data type which this doubly-
 * linked list holds. For example, if this doubly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since the data is being
 * copied, the pointer only has to be valid when this function is called.
 *
 * @param me    the doubly-linked list to set data for
 * @param index the index to set data in the doubly-linked list
 * @param data  the data to set in the doubly-linked list
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_set_at(list me, const size_t index, void *const data)
{
    char *traverse;
    if (index >= me->item_count) {
        return -EINVAL;
    }
    traverse = list_get_node_at(me, index);
    memcpy(traverse + node_data_ptr_offset, data, me->bytes_per_item);
    return 0;
}

/**
 * Sets the data at the last index in the doubly-linked list. The pointer to
 * the data being passed in should point to the data type which this doubly-
 * linked list holds. For example, if this doubly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since the data is being
 * copied, the pointer only has to be valid when this function is called.
 *
 * @param me   the doubly-linked list to set data for
 * @param data the data to set in the doubly-linked list
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_set_last(list me, void *const data)
{
    return list_set_at(me, me->item_count - 1, data);
}

/**
 * Gets the data at the first index in the doubly-linked list. The pointer to
 * the data being obtained should point to the data type which this doubly-
 * linked list holds. For example, if this doubly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since this data is being
 * copied from the array to the data pointer, the pointer only has to be valid
 * when this function is called.
 *
 * @param data the data to get
 * @param me   the doubly-linked list to get data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_get_first(void *const data, list me)
{
    return list_get_at(data, me, 0);
}

/**
 * Gets the data at the specified index in the doubly-linked list. The pointer
 * to the data being obtained should point to the data type which this doubly-
 * linked list holds. For example, if this doubly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since this data is being
 * copied from the array to the data pointer, the pointer only has to be valid
 * when this function is called.
 *
 * @param data  the data to get
 * @param me    the doubly-linked list to get data from
 * @param index the index to get data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_get_at(void *const data, list me, const size_t index)
{
    char *traverse;
    if (index >= me->item_count) {
        return -EINVAL;
    }
    traverse = list_get_node_at(me, index);
    memcpy(data, traverse + node_data_ptr_offset, me->bytes_per_item);
    return 0;
}

/**
 * Gets the data at the last index in the doubly-linked list. The pointer to
 * the data being obtained should point to the data type which this doubly-
 * linked list holds. For example, if this doubly-linked list holds integers,
 * the data pointer should be a pointer to an integer. Since this data is being
 * copied from the array to the data pointer, the pointer only has to be valid
 * when this function is called.
 *
 * @param data the data to get
 * @param me   the doubly-linked list to get data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_get_last(void *const data, list me)
{
    return list_get_at(data, me, me->item_count - 1);
}

/**
 * Clears all elements from the doubly-linked list.
 *
 * @param me the doubly-linked list to clear
 */
void list_clear(list me)
{
    char *traverse = me->head;
    while (traverse) {
        char *temp = traverse;
        memcpy(&traverse, traverse + node_next_ptr_offset, ptr_size);
        free(temp);
    }
    me->item_count = 0;
    me->head = NULL;
    me->tail = NULL;
}

/**
 * Destroys the doubly-linked list. Performing further operations after calling
 * this function results in undefined behavior.
 *
 * @param me the doubly-linked list to destroy
 *
 * @return NULL
 */
list list_destroy(list me)
{
    list_clear(me);
    free(me);
    return NULL;
}
