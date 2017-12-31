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
#include "multiset.h"

struct _multiset {
    size_t key_size;
    int (*comparator)(const void *const one, const void *const two);
    int size;
    struct node *root;
};

struct node {
    int count;
    struct node *parent;
    int balance;
    void *key;
    struct node *left;
    struct node *right;
};

/**
 * Initializes a multiset, which is a collection of key-value pairs, sorted by
 * keys, keys are unique
 *
 * @param key_size   The size of each element in the multiset.
 * @param comparator The comparator function used for key ordering.
 *
 * @return The newly-initialized multiset, or NULL if memory allocation error.
 */
multiset multiset_init(const size_t key_size,
                       int (*const comparator)(const void *const,
                                               const void *const))
{
    struct _multiset *const init = malloc(sizeof(struct _multiset));
    if (init == NULL) {
        return NULL;
    }
    init->key_size = key_size;
    init->comparator = comparator;
    init->size = 0;
    init->root = NULL;
    return init;
}

/**
 * Gets the size of the multiset.
 *
 * @param me The multiset to check.
 *
 * @return The size of the multiset.
 */
int multiset_size(multiset me)
{
    return me->size;
}

/**
 * Determines whether or not the multiset is empty.
 *
 * @param me The multiset to check.
 *
 * @return If the multiset is empty.
 */
bool multiset_is_empty(multiset me)
{
    return multiset_size(me) == 0;
}

/*
 * Resets the parent reference.
 */
static void multiset_reference_parent(multiset me,
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
static void multiset_rotate_left(multiset me,
                                 struct node *const parent,
                                 struct node *const child)
{
    multiset_reference_parent(me, parent, child);
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
static void multiset_rotate_right(multiset me,
                                  struct node *const parent,
                                  struct node *const child)
{
    multiset_reference_parent(me, parent, child);
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
static struct node *multiset_repair(multiset me,
                                    struct node *const parent,
                                    struct node *const child,
                                    struct node *const grand_child)
{
    if (parent->balance == 2 && child->balance >= 0) {
        multiset_rotate_left(me, parent, child);
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
        multiset_rotate_right(me, parent, child);
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
        multiset_rotate_left(me, child, grand_child);
        multiset_rotate_right(me, parent, grand_child);
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
        multiset_rotate_right(me, child, grand_child);
        multiset_rotate_left(me, parent, grand_child);
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
static void multiset_insert_balance(multiset me, struct node *const item)
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
            multiset_repair(me, parent, child, grand_child);
            return;
        }
        grand_child = child;
        child = parent;
        parent = parent->parent;
    }
}

/*
 * Creates and allocates a node.
 */
static struct node *multiset_create_node(multiset me,
                                         const void *const data,
                                         struct node *const parent)
{
    struct node *const insert = malloc(sizeof(struct node));
    if (insert == NULL) {
        return NULL;
    }
    insert->count = 1;
    insert->parent = parent;
    insert->balance = 0;
    insert->key = malloc(me->key_size);
    if (insert->key == NULL) {
        free(insert);
        return NULL;
    }
    memcpy(insert->key, data, me->key_size);
    insert->left = NULL;
    insert->right = NULL;
    return insert;
}

/**
 * Adds an element to the multiset if the multiset does not already contain it.
 *
 * @param me  The multiset to add to.
 * @param key The element to add.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int multiset_put(multiset me, void *const key)
{
    if (me->root == NULL) {
        struct node *insert = multiset_create_node(me, key, NULL);
        if (insert == NULL) {
            return -ENOMEM;
        }
        me->root = insert;
        me->size++;
        return 0;
    }
    struct node *traverse = me->root;
    while (true) {
        const int compare = me->comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left != NULL) {
                traverse = traverse->left;
            } else {
                struct node *insert = multiset_create_node(me, key, traverse);
                if (insert == NULL) {
                    return -ENOMEM;
                }
                traverse->left = insert;
                multiset_insert_balance(me, insert);
                me->size++;
                return 0;
            }
        } else if (compare > 0) {
            if (traverse->right != NULL) {
                traverse = traverse->right;
            } else {
                struct node *insert = multiset_create_node(me, key, traverse);
                if (insert == NULL) {
                    return -ENOMEM;
                }
                traverse->right = insert;
                multiset_insert_balance(me, insert);
                me->size++;
                return 0;
            }
        } else {
            traverse->count++;
            me->size++;
            return 0;
        }
    }
}

/*
 * If a match occurs, returns the match. Else, returns NULL.
 */
static struct node *multiset_equal_match(multiset me, const void *const key)
{
    struct node *traverse = me->root;
    if (traverse == NULL) {
        return false;
    }
    while (true) {
        const int compare = me->comparator(key, traverse->key);
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
 * Determines the count of a specific key in the multiset.
 *
 * @param me  The multiset to check for the count.
 * @param key The element to check.
 *
 * @return The count of a specific key in the multiset.
 */
int multiset_count(multiset me, void *const key)
{
    const struct node *const item = multiset_equal_match(me, key);
    if (item == NULL) {
        return 0;
    }
    return item->count;
}

/**
 * Determines if the multiset contains the specified element.
 *
 * @param me  The multiset to check for the element.
 * @param key The element to check.
 *
 * @return If the multiset contained the element.
 */
bool multiset_contains(multiset me, void *const key)
{
    return multiset_equal_match(me, key) != NULL;
}

/*
 * Repairs the AVL tree by pivoting on an item.
 */
static struct node *multiset_repair_pivot(multiset me,
                                          struct node *const item,
                                          const bool is_left_pivot)
{
    struct node *const child = is_left_pivot ? item->right : item->left;
    struct node *const grand_child =
            child->balance == 1 ? child->right : child->left;
    return multiset_repair(me, item, child, grand_child);
}

/*
 * Balances the AVL tree on deletion.
 */
static void multiset_delete_balance(multiset me,
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
        item = multiset_repair_pivot(me, item, is_left_deleted);
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
            child = multiset_repair_pivot(me, parent, parent->left == child);
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
static void multiset_remove_no_children(multiset me,
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
        multiset_delete_balance(me, parent, true);
    } else {
        parent->right = NULL;
        multiset_delete_balance(me, parent, false);
    }
}

/*
 * Removes traverse when it has one child.
 */
static void multiset_remove_one_child(multiset me,
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
        multiset_delete_balance(me, parent, true);
    } else {
        if (traverse->left != NULL) {
            parent->right = traverse->left;
            traverse->left->parent = parent;
        } else {
            parent->right = traverse->right;
            traverse->right->parent = parent;
        }
        multiset_delete_balance(me, parent, false);
    }
}

/*
 * Removes traverse when it has two children.
 */
static void multiset_remove_two_children(multiset me,
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
    multiset_delete_balance(me, parent, is_left_deleted);
}

/*
 * Removes the element from the set.
 */
static void multiset_remove_element(multiset me, struct node *const traverse)
{
    if (traverse->left == NULL && traverse->right == NULL) {
        multiset_remove_no_children(me, traverse);
    } else if (traverse->left == NULL || traverse->right == NULL) {
        multiset_remove_one_child(me, traverse);
    } else {
        multiset_remove_two_children(me, traverse);
    }
    free(traverse->key);
    free(traverse);
}

/**
 * Removes an element from the multiset if it contains it.
 *
 * @param me  The multiset to remove an element from.
 * @param key The element to remove.
 *
 * @return If the multiset contained the element.
 */
bool multiset_remove(multiset me, void *const key)
{
    struct node *const traverse = multiset_equal_match(me, key);
    if (traverse == NULL) {
        return false;
    }
    traverse->count--;
    if (traverse->count == 0) {
        multiset_remove_element(me, traverse);
    }
    me->size--;
    return true;
}

/**
 * Removes all the elements specified by the key from an multiset if it contains
 * the key.
 *
 * @param me  The multiset to remove an element from.
 * @param key The element to remove.
 *
 * @return If the multiset contained the element.
 */
bool multiset_remove_all(multiset me, void *const key)
{
    struct node *const traverse = multiset_equal_match(me, key);
    if (traverse == NULL) {
        return false;
    }
    me->size -= traverse->count;
    multiset_remove_element(me, traverse);
    return true;
}

/**
 * Clears the elements from the multiset.
 *
 * @param me The multiset to clear.
 */
void multiset_clear(multiset me)
{
    while (me->root != NULL) {
        multiset_remove_element(me, me->root);
    }
    me->size = 0;
}

/**
 * Frees the multiset memory.
 *
 * @param me The multiset to free from memory.
 *
 * @return NULL
 */
multiset multiset_destroy(multiset me)
{
    multiset_clear(me);
    free(me);
    return NULL;
}
