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
#include "set.h"

struct _set {
    size_t key_size;
    int (*comparator)(const void *const one, const void *const two);
    int size;
    struct node *root;
};

struct node {
    struct node *parent;
    int balance;
    void *key;
    struct node *left;
    struct node *right;
};

/**
 * Initializes a set, which is a collection of unique keys, sorted by keys.
 *
 * @param data_size  The size of each element in the set.
 * @param comparator The comparator function used for key ordering.
 *
 * @return The newly-initialized set, or NULL if memory allocation error.
 */
set set_init(const size_t key_size,
             int (*const comparator)(const void *const, const void *const))
{
    struct _set *const init = malloc(sizeof(struct _set));
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
 * Gets the size of the set.
 *
 * @param me The set to check.
 *
 * @return The size of the set.
 */
int set_size(set me)
{
    return me->size;
}

/**
 * Determines whether or not the set is empty.
 *
 * @param me The set to check.
 *
 * @return If the set is empty.
 */
bool set_is_empty(set me)
{
    return set_size(me) == 0;
}

/*
 * Resets the parent reference.
 */
static void set_reference_parent(set me,
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
static void set_rotate_left(set me,
                            struct node *const parent,
                            struct node *const child)
{
    set_reference_parent(me, parent, child);
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
static void set_rotate_right(set me,
                             struct node *const parent,
                             struct node *const child)
{
    set_reference_parent(me, parent, child);
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
static struct node *set_repair(set me,
                               struct node *const parent,
                               struct node *const child,
                               struct node *const grand_child)
{
    if (parent->balance == 2 && child->balance >= 0) {
        set_rotate_left(me, parent, child);
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
        set_rotate_right(me, parent, child);
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
        set_rotate_left(me, child, grand_child);
        set_rotate_right(me, parent, grand_child);
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
        set_rotate_right(me, child, grand_child);
        set_rotate_left(me, parent, grand_child);
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
static void set_insert_balance(set me, struct node *const item)
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
            set_repair(me, parent, child, grand_child);
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
static struct node *set_create_node(set me,
                                    const void *const data,
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
    memcpy(insert->key, data, me->key_size);
    insert->left = NULL;
    insert->right = NULL;
    me->size++;
    return insert;
}

/**
 * Adds an element to the set if the set does not already contain it.
 *
 * @param me   The set to add to.
 * @param data The element to add.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int set_add(set me, void *const key)
{
    if (me->root == NULL) {
        struct node *insert = set_create_node(me, key, NULL);
        if (insert == NULL) {
            return -ENOMEM;
        }
        me->root = insert;
        return 0;
    }
    struct node *traverse = me->root;
    while (true) {
        const int compare = me->comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left != NULL) {
                traverse = traverse->left;
            } else {
                struct node *insert = set_create_node(me, key, traverse);
                if (insert == NULL) {
                    return -ENOMEM;
                }
                traverse->left = insert;
                set_insert_balance(me, insert);
                return 0;
            }
        } else if (compare > 0) {
            if (traverse->right != NULL) {
                traverse = traverse->right;
            } else {
                struct node *insert = set_create_node(me, key, traverse);
                if (insert == NULL) {
                    return -ENOMEM;
                }
                traverse->right = insert;
                set_insert_balance(me, insert);
                return 0;
            }
        } else {
            return 0;
        }
    }
}

/*
 * If a match occurs, returns the match. Else, returns NULL.
 */
static struct node *set_equal_match(set me, const void *const key)
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
 * Determines if the set contains the specified element.
 *
 * @param me   The set to check for the element.
 * @param data The element to check.
 *
 * @return If the set contained the element.
 */
bool set_contains(set me, void *const key)
{
    return set_equal_match(me, key) != NULL;
}

/*
 * Repairs the AVL tree by pivoting on an item.
 */
static struct node *set_repair_pivot(set me,
                                     struct node *const item,
                                     const bool is_left_pivot)
{
    struct node *const child = is_left_pivot ? item->right : item->left;
    struct node *const grand_child =
            child->balance == 1 ? child->right : child->left;
    return set_repair(me, item, child, grand_child);
}

/*
 * Balances the AVL tree on deletion.
 */
static void set_delete_balance(set me,
                               struct node *const item,
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
        const struct node *const child =
                set_repair_pivot(me, item, is_left_deleted);
        const struct node *const parent = child->parent;
        if (parent == NULL || child->balance == -1 || child->balance == 1) {
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
            child = set_repair_pivot(me, parent, parent->left == child);
            parent = child->parent;
            // If balance is -1 or +1 after modification or the parent is NULL,
            // then the tree is balanced.
            if (parent == NULL || child->balance == -1 || child->balance == 1) {
                return;
            }
        }
        child = parent;
        parent = parent->parent;
    }
}

/*
 * Removes traverse when it has no children.
 */
static void set_remove_no_children(set me, const struct node *const traverse)
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
        set_delete_balance(me, parent, true);
    } else {
        parent->right = NULL;
        set_delete_balance(me, parent, false);
    }
}

/*
 * Removes traverse when it has one child.
 */
static void set_remove_one_child(set me, const struct node *const traverse)
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
        set_delete_balance(me, parent, true);
    } else {
        if (traverse->left != NULL) {
            parent->right = traverse->left;
            traverse->left->parent = parent;
        } else {
            parent->right = traverse->right;
            traverse->right->parent = parent;
        }
        set_delete_balance(me, parent, false);
    }
}

/*
 * Removes traverse when it has two children.
 */
static void set_remove_two_children(set me, const struct node *const traverse)
{
    bool is_left_deleted;
    struct node *item;
    if (traverse->right->left == NULL) {
        item = traverse->right;
        is_left_deleted = false;
    } else {
        item = traverse->right->left;
        while (item->left != NULL) {
            item = item->left;
        }
        item->parent->left = item->right;
        item->right = traverse->right;
        item->right->parent = item;
        is_left_deleted = true;
    }
    struct node *parent = item->parent;
    if (parent == traverse) {
        parent = item;
    }
    item->balance = traverse->balance;
    item->parent = traverse->parent;
    if (item->parent != NULL) {
        item->parent->left = item;
    }
    item->left = traverse->left;
    item->left->parent = item;
    if (item->parent == NULL) {
        me->root = item;
    }
    set_delete_balance(me, parent, is_left_deleted);
}

/*
 * Removes the element from the set.
 */
static void set_remove_element(set me, struct node *const traverse)
{
    if (traverse->left == NULL && traverse->right == NULL) {
        set_remove_no_children(me, traverse);
    } else if (traverse->left == NULL || traverse->right == NULL) {
        set_remove_one_child(me, traverse);
    } else {
        set_remove_two_children(me, traverse);
    }
    free(traverse->key);
    free(traverse);
    me->size--;
}

/**
 * Removes the element from the set if it contains it.
 *
 * @param me   The set to remove an element from.
 * @param data The element to remove.
 *
 * @return If the set contained the element.
 */
bool set_remove(set me, void *const key)
{
    struct node *const traverse = set_equal_match(me, key);
    if (traverse == NULL) {
        return false;
    }
    set_remove_element(me, traverse);
    return true;
}

/*
 * Recursively frees the node passed in.
 */
static void set_clear_root(struct node *const root)
{
    if (root->left != NULL) {
        set_clear_root(root->left);
    }
    if (root->right != NULL) {
        set_clear_root(root->right);
    }
    free(root->key);
    free(root);
}

/**
 * Clears the elements from the set.
 *
 * @param me The set to clear.
 */
void set_clear(set me)
{
    if (me->root != NULL) {
        set_clear_root(me->root);
        me->root = NULL;
        me->size = 0;
    }
}

/**
 * Frees the set memory.
 *
 * @param me The set to free from memory.
 *
 * @return NULL
 */
set set_destroy(set me)
{
    set_clear(me);
    free(me);
    return NULL;
}
