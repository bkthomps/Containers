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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "list.h"

struct internal_list {
    size_t bytes_per_item;
    int item_count;
    struct node *head;
    struct node *tail;
};

struct node {
    struct node *prev;
    void *data;
    struct node *next;
};

/**
 * Initializes a doubly-linked list.
 *
 * @param data_size the size of data to store; must be positive
 *
 * @return the newly-initialized doubly-linked list, or NULL if memory
 *         allocation error
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
 * Gets the amount of elements in the doubly-linked list.
 *
 * @param me the doubly-linked list to check
 *
 * @return the amount of elements
 */
int list_size(list me)
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
 * Copies the nodes of the doubly-linked list to an array.
 *
 * @param arr the initialized array to copy the doubly-linked list to
 * @param me  the doubly-linked list to copy to the array
 */
void list_copy_to_array(void *const arr, list me)
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
 * Get the node at index starting from the head.
 */
static struct node *list_get_node_from_head(list me, const int index)
{
    struct node *traverse = me->head;
    int i;
    for (i = 0; i < index; i++) {
        traverse = traverse->next;
    }
    return traverse;
}

/*
 * Get the node at index starting from tail.
 */
static struct node *list_get_node_from_tail(list me, const int index)
{
    struct node *traverse = me->tail;
    int i;
    for (i = me->item_count - 1; i > index; i--) {
        traverse = traverse->prev;
    }
    return traverse;
}

/*
 * Get the node at the specified index.
 */
static struct node *list_get_node_at(list me, const int index)
{
    if (index <= me->item_count / 2) {
        return list_get_node_from_head(me, index);
    } else {
        return list_get_node_from_tail(me, index);
    }
}

/**
 * Adds data at the first index in the doubly-linked list.
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
 * Adds data at a specified index in the doubly-linked list.
 *
 * @param me    the doubly-linked list to add data to
 * @param index the index to add the data at
 * @param data  the data to add to the doubly-linked list
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 * @return -EINVAL if invalid argument
 */
int list_add_at(list me, const int index, void *const data)
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
    if (!me->head) {
        add->prev = NULL;
        add->next = NULL;
        me->head = add;
        me->tail = add;
    } else if (index == 0) {
        struct node *const traverse = me->head;
        traverse->prev = add;
        add->prev = NULL;
        add->next = traverse;
        me->head = add;
    } else if (index == me->item_count) {
        struct node *const traverse = me->tail;
        traverse->next = add;
        add->prev = traverse;
        add->next = NULL;
        me->tail = add;
    } else {
        struct node *const traverse = list_get_node_at(me, index);
        add->prev = traverse->prev;
        add->next = traverse;
        traverse->prev->next = add;
        traverse->prev = add;
    }
    me->item_count++;
    return 0;
}

/**
 * Adds data at the last index in the doubly-linked list.
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

/*
 * Determines if the input is illegal.
 */
static int list_is_illegal_input(list me, const int index)
{
    return index < 0 || index >= me->item_count;
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
int list_remove_at(list me, const int index)
{
    struct node *traverse;
    if (list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    traverse = list_get_node_at(me, index);
    if (index == 0) {
        traverse->next->prev = NULL;
        me->head = traverse->next;
    } else if (index == me->item_count - 1) {
        traverse->prev->next = NULL;
        me->tail = traverse->prev;
    } else {
        traverse->prev->next = traverse->next;
        traverse->next->prev = traverse->prev;
    }
    free(traverse->data);
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
 * Sets the data at the first index in the doubly-linked list.
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
 * Sets the data at the specified index in the doubly-linked list.
 *
 * @param me    the doubly-linked list to set data for
 * @param index the index to set data in the doubly-linked list
 * @param data  the data to set in the doubly-linked list
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_set_at(list me, const int index, void *const data)
{
    struct node *traverse;
    if (list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    traverse = list_get_node_at(me, index);
    memcpy(traverse->data, data, me->bytes_per_item);
    return 0;
}

/**
 * Sets the data at the last index in the doubly-linked list.
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
 * Gets the data at the first index in the doubly-linked list.
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
 * Gets the data at the specified index in the doubly-linked list.
 *
 * @param data  the data to get
 * @param me    the doubly-linked list to get data from
 * @param index the index to get data from
 *
 * @return 0       if no error
 * @return -EINVAL if invalid argument
 */
int list_get_at(void *const data, list me, const int index)
{
    struct node *traverse;
    if (list_is_illegal_input(me, index)) {
        return -EINVAL;
    }
    traverse = list_get_node_at(me, index);
    memcpy(data, traverse->data, me->bytes_per_item);
    return 0;
}

/**
 * Gets the data at the last index in the doubly-linked list.
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
    struct node *traverse = me->head;
    while (traverse) {
        struct node *const temp = traverse;
        traverse = traverse->next;
        free(temp->data);
        free(temp);
    }
    me->head = NULL;
    me->item_count = 0;
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
