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
#include "multimap.h"

struct _multimap {
    size_t key_size;
    size_t value_size;
    int (*key_comparator)(const void *const one, const void *const two);
    int (*value_comparator)(const void *const one, const void *const two);
    int size;
    struct node *root;
    struct value_node *iterate_get;
};

struct node {
    struct node *parent;
    int balance;
    void *key;
    int value_count;
    struct value_node *head;
    struct node *left;
    struct node *right;
};

struct value_node {
    void *value;
    struct value_node *next;
};

/**
 * Initializes a multi-map, which is a collection of key-value pairs, sorted by
 * keys.
 *
 * @param key_size         The size of each key in the multi-map.
 * @param value_size       The size of each value in the multi-map.
 * @param key_comparator   The key comparator function.
 * @param value_comparator The value comparator function.
 *
 * @return The newly-initialized multi-map, or NULL if memory allocation error.
 */
multimap multimap_init(const size_t key_size,
                       const size_t value_size,
                       int (*const key_comparator)(const void *const,
                                                   const void *const),
                       int (*const value_comparator)(const void *const,
                                                     const void *const))
{
    struct _multimap *const init = malloc(sizeof(struct _multimap));
    if (init == NULL) {
        return NULL;
    }
    init->key_size = key_size;
    init->value_size = value_size;
    init->key_comparator = key_comparator;
    init->value_comparator = value_comparator;
    init->size = 0;
    init->root = NULL;
    init->iterate_get = NULL;
    return init;
}

/**
 * Gets the size of the multi-map.
 *
 * @param me The multi-map to check.
 *
 * @return The size of the multi-map.
 */
int multimap_size(multimap me)
{
    return me->size;
}

/**
 * Determines whether or not the multi-map is empty.
 *
 * @param me The multi-map to check.
 *
 * @return If the multi-map is empty.
 */
bool multimap_is_empty(multimap me)
{
    return multimap_size(me) == 0;
}

/*
 * Resets the parent reference.
 */
static void multimap_reference_parent(multimap me,
                                      struct node *const parent,
                                      struct node *const child)
{
    child->parent = parent->parent;
    if (parent->parent == NULL) {
        me->root = child;
    } else if (parent->parent->left == parent) {
        parent->parent->left = child;
    } else {
        parent->parent->right = child;
    }
}

/*
 * Rotates the AVL tree to the left.
 */
static void multimap_rotate_left(multimap me,
                                 struct node *const parent,
                                 struct node *const child)
{
    multimap_reference_parent(me, parent, child);
    struct node *const grand_child = child->left;
    if (grand_child != NULL) {
        grand_child->parent = parent;
    }
    parent->parent = child;
    parent->right = grand_child;
    child->left = parent;
}

/*
 * Rotates the AVL tree to the right.
 */
static void multimap_rotate_right(multimap me,
                                  struct node *const parent,
                                  struct node *const child)
{
    multimap_reference_parent(me, parent, child);
    struct node *const grand_child = child->right;
    if (grand_child != NULL) {
        grand_child->parent = parent;
    }
    parent->parent = child;
    parent->left = grand_child;
    child->right = parent;
}

/*
 * Repairs the AVL tree on insert.
 */
static struct node *multimap_repair(multimap me,
                                    struct node *const parent,
                                    struct node *const child,
                                    struct node *const grand_child)
{
    if (parent->balance == 2 && child->balance >= 0) {
        multimap_rotate_left(me, parent, child);
        if (child->balance == 0) {
            parent->balance = 1;
            child->balance = -1;
        } else {
            parent->balance = 0;
            child->balance = 0;
        }
        return child;
    }
    if (parent->balance == -2 && child->balance <= 0) {
        multimap_rotate_right(me, parent, child);
        if (child->balance == 0) {
            parent->balance = -1;
            child->balance = 1;
        } else {
            parent->balance = 0;
            child->balance = 0;
        }
        return child;
    }
    if (parent->balance == -2 && child->balance == 1) {
        multimap_rotate_left(me, child, grand_child);
        multimap_rotate_right(me, parent, grand_child);
        if (grand_child->balance == 1) {
            parent->balance = 0;
            child->balance = -1;
        } else if (grand_child->balance == 0) {
            parent->balance = 0;
            child->balance = 0;
        } else {
            parent->balance = 1;
            child->balance = 0;
        }
        grand_child->balance = 0;
        return grand_child;
    }
    if (parent->balance == 2 && child->balance == -1) {
        multimap_rotate_right(me, child, grand_child);
        multimap_rotate_left(me, parent, grand_child);
        if (grand_child->balance == 1) {
            parent->balance = -1;
            child->balance = 0;
        } else if (grand_child->balance == 0) {
            parent->balance = 0;
            child->balance = 0;
        } else {
            parent->balance = 0;
            child->balance = 1;
        }
        grand_child->balance = 0;
        return grand_child;
    }
    // Impossible to get here.
    return NULL;
}

/*
 * Balances the AVL tree on insert.
 */
static void multimap_insert_balance(multimap me, struct node *const item)
{
    struct node *grand_child = NULL;
    struct node *child = item;
    struct node *parent = item->parent;
    while (parent != NULL) {
        if (parent->left == child) {
            parent->balance--;
        } else {
            parent->balance++;
        }
        // If balance is zero after modification, then the tree is balanced.
        if (parent->balance == 0) {
            return;
        }
        // Must re-balance if not in {-1, 0, 1}
        if (parent->balance > 1 || parent->balance < -1) {
            // After one repair, the tree is balanced.
            multimap_repair(me, parent, child, grand_child);
            return;
        }
        grand_child = child;
        child = parent;
        parent = parent->parent;
    }
}

/*
 * Creates and allocates a value node.
 */
static struct value_node *multimap_create_value_node(multimap me,
                                                     const void *const value)
{
    struct value_node *const add = malloc(sizeof(struct value_node));
    if (add == NULL) {
        return NULL;
    }
    add->value = malloc(me->value_size);
    if (add->value == NULL) {
        free(add);
        return NULL;
    }
    memcpy(add->value, value, me->value_size);
    add->next = NULL;
    return add;
}

/*
 * Creates and allocates a node.
 */
static struct node *multimap_create_node(multimap me,
                                         const void *const key,
                                         const void *const value,
                                         struct node *const parent)
{
    struct node *const insert = malloc(sizeof(struct node));
    if (insert == NULL) {
        return NULL;
    }
    insert->parent = parent;
    insert->balance = 0;
    insert->key = malloc(me->key_size);
    if (insert->key == NULL) {
        free(insert);
        return NULL;
    }
    memcpy(insert->key, key, me->key_size);
    insert->value_count = 1;
    insert->head = multimap_create_value_node(me, value);
    if (insert->head == NULL) {
        free(insert->key);
        free(insert);
        return NULL;
    }
    insert->left = NULL;
    insert->right = NULL;
    me->size++;
    return insert;
}

/**
 * Adds a key-value pair to the multi-map.
 *
 * @param me    The multi-map to add to.
 * @param key   The key to add.
 * @param value The value to add.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int multimap_put(multimap me, void *const key, void *const value)
{
    if (me->root == NULL) {
        struct node *insert = multimap_create_node(me, key, value, NULL);
        if (insert == NULL) {
            return -ENOMEM;
        }
        me->root = insert;
        return 0;
    }
    struct node *traverse = me->root;
    while (true) {
        const int compare = me->key_comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left != NULL) {
                traverse = traverse->left;
            } else {
                struct node *insert =
                        multimap_create_node(me, key, value, traverse);
                if (insert == NULL) {
                    return -ENOMEM;
                }
                traverse->left = insert;
                multimap_insert_balance(me, insert);
                return 0;
            }
        } else if (compare > 0) {
            if (traverse->right != NULL) {
                traverse = traverse->right;
            } else {
                struct node *insert =
                        multimap_create_node(me, key, value, traverse);
                if (insert == NULL) {
                    return -ENOMEM;
                }
                traverse->right = insert;
                multimap_insert_balance(me, insert);
                return 0;
            }
        } else {
            struct value_node *value_traverse = traverse->head;
            while (value_traverse->next != NULL) {
                value_traverse = value_traverse->next;
            }
            value_traverse->next = multimap_create_value_node(me, value);
            traverse->value_count++;
            me->size++;
            return 0;
        }
    }
}

/*
 * If a match occurs, returns the match. Else, returns NULL.
 */
static struct node *multimap_equal_match(multimap me, const void *const key)
{
    struct node *traverse = me->root;
    if (traverse == NULL) {
        return false;
    }
    while (true) {
        const int compare = me->key_comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left != NULL) {
                traverse = traverse->left;
            } else {
                return NULL;
            }
        } else if (compare > 0) {
            if (traverse->right != NULL) {
                traverse = traverse->right;
            } else {
                return NULL;
            }
        } else {
            return traverse;
        }
    }
}

/**
 * Creates the iterator for the specified key. To iterate over the values, keep
 * getting the next value. Between starting and iterations, the multi-map must
 * not be mutated.
 *
 * @param me  The multi-map to start the iterator for.
 * @param key The key to start the iterator for.
 */
void multimap_get_start(multimap me, void *const key)
{
    const struct node *const traverse = multimap_equal_match(me, key);
    if (traverse != NULL) {
        me->iterate_get = traverse->head;
    }
}

/**
 * Iterates over the values for the specified key. Must be called after starting
 * the iterator. The multi-map must not be mutated between start and iterations.
 *
 * @param value The value to be copied to from iteration.
 * @param me    The multi-map to iterate over.
 *
 * @return If there exist no more values for the key which is being iterated
 *         over.
 */
bool multimap_get_next(void *const value, multimap me)
{
    if (me->iterate_get == NULL) {
        return false;
    }
    const struct value_node *const item = me->iterate_get;
    memcpy(value, item->value, me->value_size);
    me->iterate_get = item->next;
    return true;
}

/**
 * Determines the amount of times the key appears in the multi-map.
 *
 * @param me  The multi-map to check for the key.
 * @param key The key to check.
 *
 * @return The amount of times the key appears in the multi-map.
 */
int multimap_count(multimap me, void *const key)
{
    struct node *const traverse = multimap_equal_match(me, key);
    if (traverse == NULL) {
        return 0;
    }
    return traverse->value_count;
}

/**
 * Determines if the multi-map contains the specified key.
 *
 * @param me  The multi-map to check for the key.
 * @param key The key to check.
 *
 * @return If the multi-map contained the key.
 */
bool multimap_contains(multimap me, void *const key)
{
    return multimap_equal_match(me, key) != NULL;
}

/*
 * Repairs the AVL tree by pivoting on an item.
 */
static struct node *multimap_repair_pivot(multimap me,
                                          struct node *const item,
                                          const bool is_left_pivot)
{
    struct node *const child = is_left_pivot ? item->right : item->left;
    struct node *const grand_child =
            child->balance == 1 ? child->right : child->left;
    return multimap_repair(me, item, child, grand_child);
}

/*
 * Balances the AVL tree on deletion.
 */
static void multimap_delete_balance(multimap me,
                                    struct node *item,
                                    const bool is_left_deleted)
{
    if (is_left_deleted) {
        item->balance++;
    } else {
        item->balance--;
    }
    // If balance is -1 or +1 after modification, then the tree is balanced.
    if (item->balance == -1 || item->balance == 1) {
        return;
    }
    // Must re-balance if not in {-1, 0, 1}
    if (item->balance > 1 || item->balance < -1) {
        item = multimap_repair_pivot(me, item, is_left_deleted);
        if (item->parent == NULL || item->balance == -1 || item->balance == 1) {
            return;
        }
    }
    struct node *child = item;
    struct node *parent = item->parent;
    while (parent != NULL) {
        if (parent->left == child) {
            parent->balance++;
        } else {
            parent->balance--;
        }
        // If balance is -1 or +1 after modification, then the tree is balanced.
        if (parent->balance == -1 || parent->balance == 1) {
            return;
        }
        // Must re-balance if not in {-1, 0, 1}
        if (parent->balance > 1 || parent->balance < -1) {
            child = multimap_repair_pivot(me, parent, parent->left == child);
            parent = child->parent;
            // If balance is -1 or +1 after modification or the parent is NULL,
            // then the tree is balanced.
            if (parent == NULL || child->balance == -1 || child->balance == 1) {
                return;
            }
        } else {
            child = parent;
            parent = parent->parent;
        }
    }
}

/*
 * Removes traverse when it has no children.
 */
static void multimap_remove_no_children(multimap me,
                                        const struct node *const traverse)
{
    struct node *const parent = traverse->parent;
    // If no parent and no children, then the only node is traverse.
    if (parent == NULL) {
        me->root = NULL;
        return;
    }
    // No re-reference needed since traverse has no children.
    if (parent->left == traverse) {
        parent->left = NULL;
        multimap_delete_balance(me, parent, true);
    } else {
        parent->right = NULL;
        multimap_delete_balance(me, parent, false);
    }
}

/*
 * Removes traverse when it has one child.
 */
static void multimap_remove_one_child(multimap me,
                                      const struct node *const traverse)
{
    struct node *const parent = traverse->parent;
    // If no parent, make the child of traverse the new root.
    if (parent == NULL) {
        if (traverse->left != NULL) {
            traverse->left->parent = NULL;
            me->root = traverse->left;
        } else {
            traverse->right->parent = NULL;
            me->root = traverse->right;
        }
        return;
    }
    // The parent of traverse now references the child of traverse.
    if (parent->left == traverse) {
        if (traverse->left != NULL) {
            parent->left = traverse->left;
            traverse->left->parent = parent;
        } else {
            parent->left = traverse->right;
            traverse->right->parent = parent;
        }
        multimap_delete_balance(me, parent, true);
    } else {
        if (traverse->left != NULL) {
            parent->right = traverse->left;
            traverse->left->parent = parent;
        } else {
            parent->right = traverse->right;
            traverse->right->parent = parent;
        }
        multimap_delete_balance(me, parent, false);
    }
}

/*
 * Removes traverse when it has two children.
 */
static void multimap_remove_two_children(multimap me,
                                         const struct node *const traverse)
{
    struct node *item;
    struct node *parent;
    const bool is_left_deleted = traverse->right->left != NULL;
    if (!is_left_deleted) {
        item = traverse->right;
        parent = item;
        item->balance = traverse->balance;
        item->parent = traverse->parent;
        item->left = traverse->left;
        item->left->parent = item;
    } else {
        item = traverse->right->left;
        while (item->left != NULL) {
            item = item->left;
        }
        parent = item->parent;
        item->balance = traverse->balance;
        item->parent->left = item->right;
        if (item->right != NULL) {
            item->right->parent = item->parent;
        }
        item->left = traverse->left;
        item->left->parent = item;
        item->right = traverse->right;
        item->right->parent = item;
        item->parent = traverse->parent;
    }
    if (traverse->parent == NULL) {
        me->root = item;
    } else if (traverse->parent->left == traverse) {
        item->parent->left = item;
    } else {
        item->parent->right = item;
    }
    multimap_delete_balance(me, parent, is_left_deleted);
}

/*
 * Removes the element from the map.
 */
static void multimap_remove_element(multimap me, struct node *const traverse)
{
    if (traverse->left == NULL && traverse->right == NULL) {
        multimap_remove_no_children(me, traverse);
    } else if (traverse->left == NULL || traverse->right == NULL) {
        multimap_remove_one_child(me, traverse);
    } else {
        multimap_remove_two_children(me, traverse);
    }
    free(traverse->key);
    free(traverse);
}

/**
 * Removes the key-value pair from the multi-map if it contains it.
 *
 * @param me    The multi-map to remove an key from.
 * @param key   The key to remove.
 * @param value The value to remove.
 *
 * @return If the multi-map contained the key.
 */
bool multimap_remove(multimap me, void *const key, void *const value)
{
    struct node *const traverse = multimap_equal_match(me, key);
    if (traverse == NULL) {
        return false;
    }
    struct value_node *current = traverse->head;
    if (me->value_comparator(current->value, value) == 0) {
        traverse->head = current->next;
    } else {
        struct value_node *previous = current;
        current = current->next;
        while (current != NULL &&
               me->value_comparator(current->value, value) != 0) {
            previous = current;
            current = current->next;
        }
        if (current == NULL) {
            return false;
        }
        previous->next = current->next;
    }
    free(current->value);
    free(current);
    traverse->value_count--;
    if (traverse->value_count == 0) {
        multimap_remove_element(me, traverse);
    }
    me->size--;
    return true;
}

/*
 * Removes all values associated with a key.
 */
static void multimap_remove_all_element(multimap me,
                                        struct node *const traverse)
{
    struct value_node *value_traverse = traverse->head;
    while (value_traverse != NULL) {
        struct value_node *temp = value_traverse;
        value_traverse = value_traverse->next;
        free(temp->value);
        free(temp);
    }
    me->size -= traverse->value_count;
    multimap_remove_element(me, traverse);
}

/**
 * Removes all the key-value pairs from the multi-map specified by the key.
 *
 * @param me  The multi-map to remove a key-value pair from.
 * @param key The key to remove.
 *
 * @return If the multi-map contained the key.
 */
bool multimap_remove_all(multimap me, void *const key)
{
    struct node *const traverse = multimap_equal_match(me, key);
    if (traverse == NULL) {
        return false;
    }
    multimap_remove_all_element(me, traverse);
    return true;
}

/**
 * Clears the key-value pairs from the multi-map.
 *
 * @param me The multi-map to clear.
 */
void multimap_clear(multimap me)
{
    while (me->root != NULL) {
        multimap_remove_all_element(me, me->root);
    }
}

/**
 * Frees the multi-map memory.
 *
 * @param me The multi-map to free from memory.
 *
 * @return NULL
 */
multimap multimap_destroy(multimap me)
{
    multimap_clear(me);
    free(me);
    return NULL;
}
