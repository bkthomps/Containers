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
#include "forward_list.h"

struct _forward_list {
    size_t data_size;
    int space;
    struct node *head;
};

struct node {
    void *data;
    struct node *next;
};

/**
 * Initializes a singly-linked list.
 *
 * @param data_size The size of data to store.
 *
 * @return The singly-linked list, or NULL if memory could not be allocated.
 */
forward_list forward_list_init(const size_t data_size)
{
    struct _forward_list *const init = malloc(sizeof(struct _forward_list));
    if (init == NULL) {
        return NULL;
    }
    init->data_size = data_size;
    init->space = 0;
    init->head = NULL;
    return init;
}

/**
 * Gets the amount of elements in the singly-linked list.
 *
 * @param me The singly-linked list to check.
 *
 * @return The amount of elements.
 */
int forward_list_size(forward_list me)
{
    return me->space;
}

/**
 * Determines if the singly-linked list is empty.
 *
 * @param me The singly-linked list to check.
 *
 * @return If the list is empty.
 */
bool forward_list_is_empty(forward_list me)
{
    return forward_list_size(me) == 0;
}

/**
 * Copies the nodes of the singly-linked list to an array.
 *
 * @param array The array to copy the list to.
 * @param me    The list to copy to the array.
 */
void forward_list_to_array(void *const array, forward_list me)
{
    struct node *traverse = me->head;
    int offset = 0;
    while (traverse != NULL) {
        memcpy(array + offset, traverse->data, me->data_size);
        offset += me->data_size;
        traverse = traverse->next;
    }
}

/*
 * Get the node at the specified index.
 */
static struct node *forward_list_get_node_at(forward_list me, const int index)
{
    struct node *traverse = me->head;
    for (int i = 0; i < index; i++) {
        traverse = traverse->next;
    }
    return traverse;
}

/**
 * Adds data at the first index in the singly-linked list.
 *
 * @param me   The list to add data to.
 * @param data The data to add to the list.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int forward_list_add_first(forward_list me, void *const data)
{
    return forward_list_add_at(me, 0, data);
}

/**
 * Adds data at a specified index in the singly-linked list.
 *
 * @param me    The list to add data to.
 * @param index The index to add the data at.
 * @param data  The data to add to the list.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 *         -EINVAL Invalid argument.
 */
int forward_list_add_at(forward_list me, const int index, void *const data)
{
    if (index < 0 || index > me->space) {
        return -EINVAL;
    }
    struct node *const add = malloc(sizeof(struct node));
    if (add == NULL) {
        return -ENOMEM;
    }
    add->data = malloc(me->data_size);
    if (add->data == NULL) {
        free(add);
        return -ENOMEM;
    }
    memcpy(add->data, data, me->data_size);
    if (index == 0) {
        add->next = me->head;
        me->head = add;
    } else {
        struct node *const traverse = forward_list_get_node_at(me, index - 1);
        add->next = traverse->next;
        traverse->next = add;
    }
    me->space++;
    return 0;
}

/**
 * Adds data at the last index in the singly-linked list.
 *
 * @param me   The list to add data to.
 * @param data The data to add to the list.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int forward_list_add_last(forward_list me, void *const data)
{
    return forward_list_add_at(me, me->space, data);
}

/*
 * Determines if the input is illegal.
 */
static bool forward_list_is_illegal_input(forward_list me, const int index)
{
    return index < 0 || index >= me->space || me->space == 0;
}

/**
 * Removes the first piece of data from the singly-linked list.
 *
 * @param me The list to remove data from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_remove_first(forward_list me)
{
    return forward_list_remove_at(me, 0);
}

/**
 * Removes data from the singly-linked list at the specified index.
 *
 * @param me    The list to remove data from.
 * @param index The index to remove from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_remove_at(forward_list me, const int index)
{
    if (forward_list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    if (index == 0) {
        struct node *const backup_head = me->head;
        me->head = me->head->next;
        free(backup_head);
    } else if (index == me->space - 1) {
        struct node *const traverse = forward_list_get_node_at(me, index - 1);
        free(traverse->next);
        traverse->next = NULL;
    } else {
        struct node *const traverse = forward_list_get_node_at(me, index - 1);
        struct node *const backup = traverse->next;
        traverse->next = traverse->next->next;
        free(backup);
    }
    me->space--;
    return 0;
}

/**
 * Removes the last piece of data from the singly-linked list.
 *
 * @param me The list to remove data from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_remove_last(forward_list me)
{
    return forward_list_remove_at(me, me->space - 1);
}

/**
 * Sets the data at the first index in the singly-linked list.
 *
 * @param me   The list to set data for.
 * @param data The data to set in the list.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_set_first(forward_list me, void *const data)
{
    return forward_list_set_at(me, 0, data);
}

/**
 * Sets the data at the specified index in the singly-linked list.
 *
 * @param me    The list to set data for.
 * @param index The index to set data in the list.
 * @param data  The data to set in the list.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_set_at(forward_list me, const int index, void *const data)
{
    if (forward_list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    struct node *const traverse = forward_list_get_node_at(me, index);
    memcpy(traverse->data, data, me->data_size);
    return 0;
}

/**
 * Sets the data at the last index in the singly-linked list.
 *
 * @param me   The list to set data for.
 * @param data The data to set in the list.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_set_last(forward_list me, void *const data)
{
    return forward_list_set_at(me, me->space - 1, data);
}

/**
 * Gets the data at the first index in the singly-linked list.
 *
 * @param data The data to get.
 * @param me   The list to get data from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_get_first(void *const data, forward_list me)
{
    return forward_list_get_at(data, me, 0);
}

/**
 * Gets the data at the specified index in the singly-linked list.
 *
 * @param data  The data to get.
 * @param me    The list to get data from.
 * @param index The index to get data from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_get_at(void *const data, forward_list me, const int index)
{
    if (forward_list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    struct node *const traverse = forward_list_get_node_at(me, index);
    memcpy(data, traverse->data, me->data_size);
    return 0;
}

/**
 * Gets the data at the last index in the singly-linked list.
 *
 * @param data The data to get.
 * @param me   The list to get data from.
 *
 * @return 0       No error.
 *         -EINVAL Invalid argument.
 */
int forward_list_get_last(void *const data, forward_list me)
{
    return forward_list_get_at(data, me, me->space - 1);
}

/**
 * Clears all elements from the singly-linked list.
 *
 * @param me The list to clear.
 */
void forward_list_clear(forward_list me)
{
    struct node *traverse = me->head;
    while (traverse != NULL) {
        struct node *const temp = traverse;
        traverse = traverse->next;
        free(temp);
    }
    me->head = NULL;
    me->space = 0;
}

/**
 * Destroys the singly-linked list.
 *
 * @param me The list to destroy.
 *
 * @return NULL
 */
forward_list forward_list_destroy(forward_list me)
{
    forward_list_clear(me);
    free(me);
    return NULL;
}
