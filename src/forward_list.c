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
    int item_count;
    struct node *head;
};

struct node {
    void *data;
    struct node *next;
};

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
    return init;
}

/**
 * Gets the amount of elements in the singly-linked list.
 *
 * @param me the singly-linked list to check
 *
 * @return the amount of elements
 */
int forward_list_size(forward_list me)
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
 * Copies the nodes of the singly-linked list to an array.
 *
 * @param arr the initialized array to copy the singly-linked list to
 * @param me  the singly-linked list to copy to the array
 */
void forward_list_copy_to_array(void *const arr, forward_list me)
{
    struct node *traverse = me->head;
    int offset = 0;
    while (traverse) {
        memcpy((char *) arr + offset, traverse->data, me->bytes_per_item);
        offset += me->bytes_per_item;
        traverse = traverse->next;
    }
}

/*
 * Get the node at the specified index.
 */
static struct node *forward_list_get_node_at(forward_list me, const int index)
{
    struct node *traverse = me->head;
    int i;
    for (i = 0; i < index; i++) {
        traverse = traverse->next;
    }
    return traverse;
}

/**
 * Adds data at the first index in the singly-linked list.
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
 * Adds data at a specified index in the singly-linked list.
 *
 * @param me    the singly-linked list to add data to
 * @param index the index to add the data at
 * @param data  the data to add to the singly-linked list
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 * @return -EINVAL if invalid argument
 */
int forward_list_add_at(forward_list me, const int index, void *const data)
{
    struct node *add;
    if (index < 0 || index > me->item_count) {
        return -EINVAL;
    }
    add = malloc(sizeof(struct node));
    if (!add) {
        return -ENOMEM;
    }
    add->data = malloc(me->bytes_per_item);
    if (!add->data) {
        free(add);
        return -ENOMEM;
    }
    memcpy(add->data, data, me->bytes_per_item);
    if (index == 0) {
        add->next = me->head;
        me->head = add;
    } else {
        struct node *const traverse = forward_list_get_node_at(me, index - 1);
        add->next = traverse->next;
        traverse->next = add;
    }
    me->item_count++;
    return 0;
}

/**
 * Adds data at the last index in the singly-linked list.
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

/*
 * Determines if the input is illegal.
 */
static int forward_list_is_illegal_input(forward_list me, const int index)
{
    return index < 0 || index >= me->item_count;
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
int forward_list_remove_at(forward_list me, const int index)
{
    if (forward_list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    if (index == 0) {
        struct node *const temp = me->head;
        me->head = temp->next;
        free(temp->data);
        free(temp);
    } else if (index == me->item_count - 1) {
        struct node *const traverse = forward_list_get_node_at(me, index - 1);
        free(traverse->next->data);
        free(traverse->next);
        traverse->next = NULL;
    } else {
        struct node *const traverse = forward_list_get_node_at(me, index - 1);
        struct node *const backup = traverse->next;
        traverse->next = traverse->next->next;
        free(backup->data);
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
 * Sets the data at the first index in the singly-linked list.
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
 * Sets the data at the specified index in the singly-linked list.
 *
 * @param me    the singly-linked list to set data for
 * @param index the index to set data in the singly-linked list
 * @param data  the data to set in the singly-linked list
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_set_at(forward_list me, const int index, void *const data)
{
    struct node *traverse;
    if (forward_list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    traverse = forward_list_get_node_at(me, index);
    memcpy(traverse->data, data, me->bytes_per_item);
    return 0;
}

/**
 * Sets the data at the last index in the singly-linked list.
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
 * Gets the data at the first index in the singly-linked list.
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
 * Gets the data at the specified index in the singly-linked list.
 *
 * @param data  the data to get
 * @param me    the singly-linked list to get data from
 * @param index the index to get data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int forward_list_get_at(void *const data, forward_list me, const int index)
{
    struct node *traverse;
    if (forward_list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    traverse = forward_list_get_node_at(me, index);
    memcpy(data, traverse->data, me->bytes_per_item);
    return 0;
}

/**
 * Gets the data at the last index in the singly-linked list.
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
    struct node *traverse = me->head;
    while (traverse) {
        struct node *const temp = traverse;
        traverse = traverse->next;
        free(temp->data);
        free(temp);
    }
    me->head = NULL;
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
