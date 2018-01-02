/*
 * Copyright (c) 2017-2018 Bailey Thompson
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
#include "map.h"

struct _map {
    size_t key_size;
    size_t value_size;
    int (*comparator)(const void *const one, const void *const two);
    int size;
    struct node *root;
};

struct node {
    struct node *parent;
    int balance;
    void *key;
    void *value;
    struct node *left;
    struct node *right;
};

/**
 * Initializes a map, which is a collection of key-value pairs, sorted by keys,
 * keys are unique
 *
 * @param key_size   The size of each key in the map.
 * @param value_size The size of each value in the map.
 * @param comparator The comparator function used for key ordering.
 *
 * @return The newly-initialized map, or NULL if memory allocation error.
 */
map map_init(const size_t key_size,
             const size_t value_size,
             int (*const comparator)(const void *const, const void *const))
{
    struct _map *const init = malloc(sizeof(struct _map));
    if (!init) {
        return NULL;
    }
    init->key_size = key_size;
    init->value_size = value_size;
    init->comparator = comparator;
    init->size = 0;
    init->root = NULL;
    return init;
}

/**
 * Gets the size of the map.
 *
 * @param me The map to check.
 *
 * @return The size of the map.
 */
int map_size(map me)
{
    return me->size;
}

/**
 * Determines whether or not the map is empty.
 *
 * @param me The map to check.
 *
 * @return If the map is empty.
 */
bool map_is_empty(map me)
{
    return map_size(me) == 0;
}

/*
 * Resets the parent reference.
 */
static void map_reference_parent(map me,
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
static void map_rotate_left(map me,
                            struct node *const parent,
                            struct node *const child)
{
    map_reference_parent(me, parent, child);
    struct node *const grand_child = child->left;
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
static void map_rotate_right(map me,
                             struct node *const parent,
                             struct node *const child)
{
    map_reference_parent(me, parent, child);
    struct node *const grand_child = child->right;
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
static struct node *map_repair(map me,
                               struct node *const parent,
                               struct node *const child,
                               struct node *const grand_child)
{
    if (parent->balance == 2 && child->balance >= 0) {
        map_rotate_left(me, parent, child);
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
        map_rotate_right(me, parent, child);
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
        map_rotate_left(me, child, grand_child);
        map_rotate_right(me, parent, grand_child);
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
        map_rotate_right(me, child, grand_child);
        map_rotate_left(me, parent, grand_child);
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
static void map_insert_balance(map me, struct node *const item)
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
            map_repair(me, parent, child, grand_child);
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
static struct node *map_create_node(map me,
                                    const void *const key,
                                    const void *const value,
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
    memcpy(insert->key, key, me->key_size);
    insert->value = malloc(me->value_size);
    if (!insert->value) {
        free(insert->key);
        free(insert);
        return NULL;
    }
    memcpy(insert->value, value, me->value_size);
    insert->left = NULL;
    insert->right = NULL;
    me->size++;
    return insert;
}

/**
 * Adds a key-value pair to the map. If the map already contains the key, the
 * value is updated to the new value.
 *
 * @param me    The map to add to.
 * @param key   The key to add.
 * @param value The value to add.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int map_put(map me, void *const key, void *const value)
{
    if (!me->root) {
        struct node *insert = map_create_node(me, key, value, NULL);
        if (!insert) {
            return -ENOMEM;
        }
        me->root = insert;
        return 0;
    }
    struct node *traverse = me->root;
    while (true) {
        const int compare = me->comparator(key, traverse->key);
        if (compare < 0) {
            if (traverse->left) {
                traverse = traverse->left;
            } else {
                struct node *insert = map_create_node(me, key, value, traverse);
                if (!insert) {
                    return -ENOMEM;
                }
                traverse->left = insert;
                map_insert_balance(me, insert);
                return 0;
            }
        } else if (compare > 0) {
            if (traverse->right) {
                traverse = traverse->right;
            } else {
                struct node *insert = map_create_node(me, key, value, traverse);
                if (!insert) {
                    return -ENOMEM;
                }
                traverse->right = insert;
                map_insert_balance(me, insert);
                return 0;
            }
        } else {
            memcpy(traverse->value, value, me->value_size);
            return 0;
        }
    }
}

/*
 * If a match occurs, returns the match. Else, returns NULL.
 */
static struct node *map_equal_match(map me, const void *const key)
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
 * Gets the value associated with a key in the map.
 *
 * @param value The value to copy to.
 * @param me    The map to get from.
 * @param key   The key to search for.
 *
 * @return If the map contained the key-value pair.
 */
bool map_get(void *const value, map me, void *const key)
{
    struct node *const traverse = map_equal_match(me, key);
    if (!traverse) {
        return false;
    }
    memcpy(value, traverse->value, me->value_size);
    return true;
}

/**
 * Determines if the map contains the specified key.
 *
 * @param me  The map to check for the element.
 * @param key The key to check.
 *
 * @return If the map contained the element.
 */
bool map_contains(map me, void *const key)
{
    return map_equal_match(me, key) != NULL;
}

/*
 * Repairs the AVL tree by pivoting on an item.
 */
static struct node *map_repair_pivot(map me,
                                     struct node *const item,
                                     const bool is_left_pivot)
{
    struct node *const child = is_left_pivot ? item->right : item->left;
    struct node *const grand_child =
            child->balance == 1 ? child->right : child->left;
    return map_repair(me, item, child, grand_child);
}

/*
 * Balances the AVL tree on deletion.
 */
static void map_delete_balance(map me,
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
        item = map_repair_pivot(me, item, is_left_deleted);
        if (!item->parent || item->balance == -1 || item->balance == 1) {
            return;
        }
    }
    struct node *child = item;
    struct node *parent = item->parent;
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
            child = map_repair_pivot(me, parent, parent->left == child);
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
static void map_remove_no_children(map me, const struct node *const traverse)
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
        map_delete_balance(me, parent, true);
    } else {
        parent->right = NULL;
        map_delete_balance(me, parent, false);
    }
}

/*
 * Removes traverse when it has one child.
 */
static void map_remove_one_child(map me, const struct node *const traverse)
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
        map_delete_balance(me, parent, true);
    } else {
        if (traverse->left) {
            parent->right = traverse->left;
            traverse->left->parent = parent;
        } else {
            parent->right = traverse->right;
            traverse->right->parent = parent;
        }
        map_delete_balance(me, parent, false);
    }
}

/*
 * Removes traverse when it has two children.
 */
static void map_remove_two_children(map me, const struct node *const traverse)
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
    map_delete_balance(me, parent, is_left_deleted);
}

/*
 * Removes the element from the map.
 */
static void map_remove_element(map me, struct node *const traverse)
{
    if (!traverse->left && !traverse->right) {
        map_remove_no_children(me, traverse);
    } else if (!traverse->left || !traverse->right) {
        map_remove_one_child(me, traverse);
    } else {
        map_remove_two_children(me, traverse);
    }
    free(traverse->key);
    free(traverse->value);
    free(traverse);
    me->size--;
}

/**
 * Removes the key-value pair from the map if it contains it.
 *
 * @param me  The map to remove an element from.
 * @param key The key to remove.
 *
 * @return If the map contained the key-value pair.
 */
bool map_remove(map me, void *const key)
{
    struct node *const traverse = map_equal_match(me, key);
    if (!traverse) {
        return false;
    }
    map_remove_element(me, traverse);
    return true;
}

/**
 * Clears the key-value pairs from the map.
 *
 * @param me The map to clear.
 */
void map_clear(map me)
{
    while (me->root) {
        map_remove_element(me, me->root);
    }
}

/**
 * Frees the map memory.
 *
 * @param me The map to free from memory.
 *
 * @return NULL
 */
map map_destroy(map me)
{
    map_clear(me);
    free(me);
    return NULL;
}
