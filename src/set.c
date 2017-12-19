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
// Remove when released
#include <assert.h>
#include <stdio.h>

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

static void set_dump_recursive(struct node *item, const int depth)
{
    if (item == NULL) {
        return;
    }
    printf("\n");
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    int *key_val = item->key;
    printf("%d,%d", *key_val, item->balance);
    set_dump_recursive(item->left, depth + 1);
    set_dump_recursive(item->right, depth + 1);
}

static void set_dump(set me)
{
    struct node *const item = me->root;
    if (item != NULL) {
        int *key_val = item->key;
        printf("root -> %d,%d", *key_val, item->balance);
        assert(item->parent == NULL);
    } else {
        printf("root -> NULL");
    }
    set_dump_recursive(me->root, 0);
    printf("\n");
}

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
 * Makes the parent of the affected node point to it.
 */
static void set_insert_reference_parent(set me,
                                        struct node *const parent,
                                        struct node *const child)
{
    if (parent->parent == NULL) {
        assert(parent == me->root);
        me->root = child;
    } else if (parent->parent->left == parent) {
        parent->parent->left = child;
    } else {
        assert(parent->parent->right == parent);
        parent->parent->right = child;
    }
    child->parent = parent->parent;
    parent->parent = child;
}

/*
 * Rotates two nodes left when inserting.
 */
static void set_insert_rotate_left(set me,
                                   struct node *const parent,
                                   struct node *const child)
{
    set_insert_reference_parent(me, parent, child);
    parent->left = child->right;
    child->right = parent;
}

/*
 * Rotates two nodes right when inserting.
 */
static void set_insert_rotate_right(set me,
                                    struct node *const parent,
                                    struct node *const child)
{
    set_insert_reference_parent(me, parent, child);
    parent->right = child->left;
    child->left = parent;
}

/*
 * Repairs the AVL tree on insert.
 */
static void set_insert_repair(set me,
                              struct node *const parent,
                              struct node *const child,
                              struct node *const grand_child)
{
    assert(grand_child != NULL);
    if (parent->balance == -2 && child->balance == -1) {
        set_insert_rotate_left(me, parent, child);
        assert(parent->parent == child);
    } else if (parent->balance == 2 && child->balance == 1) {
        set_insert_rotate_right(me, parent, child);
        assert(parent->parent == child);
    } else if (parent->balance == -2 && child->balance == 1) {
        set_insert_rotate_right(me, child, grand_child);
        set_insert_rotate_left(me, parent, grand_child);
        assert(parent->parent == grand_child);
        assert(child->parent == grand_child);
    } else {
        assert(parent->balance == 2 && child->balance == -1);
        set_insert_rotate_left(me, child, grand_child);
        set_insert_rotate_right(me, parent, grand_child);
        assert(parent->parent == grand_child);
        assert(child->parent == grand_child);
    }
    if (parent->left != NULL) {
        parent->left->parent = parent;
    }
    if (parent->right != NULL) {
        parent->right->parent = parent;
    }
    parent->balance = 0;
    child->balance = 0;
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
        // Must re-balance if not in {-1, 0, 1}
        if (parent->balance > 1 || parent->balance < -1) {
            set_insert_repair(me, parent, child, grand_child);
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
                                    void *const data,
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
    if (me->root == NULL) {
        return false;
    }
    struct node *traverse = me->root;
    while (true) {
        const int compare = me->comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left != NULL) {
                traverse = traverse->left;
            } else {
                return false;
            }
        } else if (compare > 0) {
            if (traverse->right != NULL) {
                traverse = traverse->right;
            } else {
                return false;
            }
        } else {
            return true;
        }
        assert(traverse->left == NULL || traverse->left->parent == traverse);
        assert(traverse->right == NULL || traverse->right->parent == traverse);
    }
}

/*
 * Removes a node and frees the key for a node which has two children.
 */
static struct node *set_remove_parent_two_children(struct node *traverse)
{
    struct node *const item = traverse;
    if (item->right->left == NULL) {
        traverse = item->right;
        item->right = traverse->right;
    } else {
        struct node *parent = traverse->right;
        traverse = traverse->right->left;
        while (traverse->left != NULL) {
            parent = traverse;
            traverse = traverse->left;
        }
        parent->left = traverse->right;
    }
    free(item->key);
    item->key = traverse->key;
    return traverse;
}

/*
 * Removes the root node of the set and replaces it with another node.
 */
static void set_remove_root(set me)
{
    struct node *temp = me->root;
    if (temp->left == NULL && temp->right == NULL) {
        me->root = NULL;
        free(temp->key);
    } else if (temp->left == NULL || temp->right == NULL) {
        if (temp->left != NULL) {
            me->root = temp->left;
        } else {
            me->root = temp->right;
        }
        free(temp->key);
    } else {
        temp = set_remove_parent_two_children(temp);
    }
    me->size--;
    free(temp);
}

/*
 * Removes a node which is not the root and replaces it with another node.
 */
static void set_remove_non_root(set me,
                                struct node *const parent,
                                struct node *traverse)
{
    if (traverse->left == NULL && traverse->right == NULL) {
        if (parent->left == traverse) {
            parent->left = NULL;
        } else {
            parent->right = NULL;
        }
        free(traverse->key);
    } else if (traverse->left == NULL || traverse->right == NULL) {
        if (parent->left == traverse) {
            if (traverse->left != NULL) {
                parent->left = traverse->left;
                traverse->left->parent = parent;
            } else {
                parent->left = traverse->right;
                traverse->right->parent = parent;
            }
        } else {
            if (traverse->left != NULL) {
                parent->right = traverse->left;
                traverse->left->parent = parent;
            } else {
                parent->right = traverse->right;
                traverse->right->parent = parent;
            }
        }
        free(traverse->key);
    } else {
        traverse = set_remove_parent_two_children(traverse);
    }
    me->size--;
    free(traverse);
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
    if (me->root == NULL) {
        return false;
    }
    struct node *parent = NULL;
    struct node *traverse = me->root;
    if (me->comparator(key, traverse->key) == 0) {
        set_remove_root(me);
        struct node *const new_root = me->root;
        if (new_root != NULL) {
            new_root->parent = NULL;
        }
        return true;
    }
    while (true) {
        const int compare = me->comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left != NULL) {
                parent = traverse;
                traverse = traverse->left;
            } else {
                return false;
            }
        } else if (compare > 0) {
            if (traverse->right != NULL) {
                parent = traverse;
                traverse = traverse->right;
            } else {
                return false;
            }
        } else {
            set_remove_non_root(me, parent, traverse);
            return true;
        }
    }
}

/*
 * Recursively frees the passed in node.
 */
static void set_clear_root(struct node *const root)
{
    if (root->left != NULL) {
        assert(root->left->parent == root);
        set_clear_root(root->left);
    }
    if (root->right != NULL) {
        assert(root->right->parent == root);
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
