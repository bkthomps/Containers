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

struct internal_set {
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
 * Initializes a set.
 *
 * @param key_size   the size of each element in the set; must be positive
 * @param comparator the comparator function used for key ordering; must not be
 *                   NULL
 *
 * @return the newly-initialized set, or NULL if memory allocation error
 */
set set_init(const size_t key_size,
             int (*const comparator)(const void *const, const void *const))
{
    struct internal_set *init;
    if (key_size == 0 || !comparator) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_set));
    if (!init) {
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
 * @param me the set to check
 *
 * @return the size of the set
 */
int set_size(set me)
{
    return me->size;
}

/**
 * Determines whether or not the set is empty.
 *
 * @param me the set to check
 *
 * @return true if the set is empty
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
    if (!parent->parent) {
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
    struct node *grand_child;
    set_reference_parent(me, parent, child);
    grand_child = child->left;
    if (grand_child) {
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
    struct node *grand_child;
    set_reference_parent(me, parent, child);
    grand_child = child->right;
    if (grand_child) {
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
    while (parent) {
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
    if (!insert) {
        return NULL;
    }
    insert->parent = parent;
    insert->balance = 0;
    insert->key = malloc(me->key_size);
    if (!insert->key) {
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
 * Adds a key to the set if the set does not already contain it.
 *
 * @param me  the set to add to
 * @param key the key to add
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int set_put(set me, void *const key)
{
    struct node *traverse;
    if (!me->root) {
        struct node *insert = set_create_node(me, key, NULL);
        if (!insert) {
            return -ENOMEM;
        }
        me->root = insert;
        return 0;
    }
    traverse = me->root;
    while (true) {
        const int compare = me->comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left) {
                traverse = traverse->left;
            } else {
                struct node *insert = set_create_node(me, key, traverse);
                if (!insert) {
                    return -ENOMEM;
                }
                traverse->left = insert;
                set_insert_balance(me, insert);
                return 0;
            }
        } else if (compare > 0) {
            if (traverse->right) {
                traverse = traverse->right;
            } else {
                struct node *insert = set_create_node(me, key, traverse);
                if (!insert) {
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
    if (!traverse) {
        return false;
    }
    while (true) {
        const int compare = me->comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left) {
                traverse = traverse->left;
            } else {
                return NULL;
            }
        } else if (compare > 0) {
            if (traverse->right) {
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
 * Determines if the set contains the specified key.
 *
 * @param me  the set to check for the key
 * @param key the key to check
 *
 * @return true if the set contained the key
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
                               struct node *item,
                               const bool is_left_deleted)
{
    struct node *child;
    struct node *parent;
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
        item = set_repair_pivot(me, item, is_left_deleted);
        if (!item->parent || item->balance == -1 || item->balance == 1) {
            return;
        }
    }
    child = item;
    parent = item->parent;
    while (parent) {
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
            if (!parent || child->balance == -1 || child->balance == 1) {
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
static void set_remove_no_children(set me, const struct node *const traverse)
{
    struct node *const parent = traverse->parent;
    // If no parent and no children, then the only node is traverse.
    if (!parent) {
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
    if (!parent) {
        if (traverse->left) {
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
        if (traverse->left) {
            parent->left = traverse->left;
            traverse->left->parent = parent;
        } else {
            parent->left = traverse->right;
            traverse->right->parent = parent;
        }
        set_delete_balance(me, parent, true);
    } else {
        if (traverse->left) {
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
        while (item->left) {
            item = item->left;
        }
        parent = item->parent;
        item->balance = traverse->balance;
        item->parent->left = item->right;
        if (item->right) {
            item->right->parent = item->parent;
        }
        item->left = traverse->left;
        item->left->parent = item;
        item->right = traverse->right;
        item->right->parent = item;
        item->parent = traverse->parent;
    }
    if (!traverse->parent) {
        me->root = item;
    } else if (traverse->parent->left == traverse) {
        item->parent->left = item;
    } else {
        item->parent->right = item;
    }
    set_delete_balance(me, parent, is_left_deleted);
}

/*
 * Removes the element from the set.
 */
static void set_remove_element(set me, struct node *const traverse)
{
    if (!traverse->left && !traverse->right) {
        set_remove_no_children(me, traverse);
    } else if (!traverse->left || !traverse->right) {
        set_remove_one_child(me, traverse);
    } else {
        set_remove_two_children(me, traverse);
    }
    free(traverse->key);
    free(traverse);
    me->size--;
}

/**
 * Removes the key from the set if it contains it.
 *
 * @param me  the set to remove an key from
 * @param key the key to remove
 *
 * @return true if the set contained the key
 */
bool set_remove(set me, void *const key)
{
    struct node *const traverse = set_equal_match(me, key);
    if (!traverse) {
        return false;
    }
    set_remove_element(me, traverse);
    return true;
}

/**
 * Clears the keys from the set.
 *
 * @param me the set to clear
 */
void set_clear(set me)
{
    while (me->root) {
        set_remove_element(me, me->root);
    }
}

/**
 * Frees the set memory.
 *
 * @param me the set to free from memory
 *
 * @return NULL
 */
set set_destroy(set me)
{
    set_clear(me);
    free(me);
    return NULL;
}
