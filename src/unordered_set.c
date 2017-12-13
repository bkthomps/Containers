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
#include "unordered_set.h"

static const int STARTING_BUCKETS = 8;
static const double RESIZE_AT = 0.75;
static const double RESIZE_RATIO = 1.5;

struct _unordered_set {
    size_t key_size;
    unsigned long (*hash)(const void *const key);
    int (*comparator)(const void *const one, const void *const two);
    int size;
    int capacity;
    struct node **buckets;
};

struct node {
    void *key;
    unsigned long hash;
    struct node *next;
};

/**
 * Initializes an unordered set, which is a collection of unique keys, hashed by
 * keys.
 *
 * @param key_size   The size of each element in the unordered set.
 * @param hash       The hash function which computes the hash from the key.
 * @param comparator The comparator function which compares two keys.
 *
 * @return The newly-initialized unordered set, or NULL if memory allocation
 *         error.
 */
unordered_set unordered_set_init(const size_t key_size,
                                 unsigned long (*hash)(const void *const),
                                 int (*comparator)(const void *const,
                                                   const void *const))
{
    struct _unordered_set *const init = malloc(sizeof(struct _unordered_set));
    if (init == NULL) {
        return NULL;
    }
    init->key_size = key_size;
    init->hash = hash;
    init->comparator = comparator;
    init->size = 0;
    init->capacity = STARTING_BUCKETS;
    init->buckets = calloc(STARTING_BUCKETS, sizeof(struct node *));
    if (init->buckets == NULL) {
        free(init);
        return NULL;
    }
    return init;
}

/*
 * Adds the specified node to the set.
 */
static void unordered_set_add_item(unordered_set me, struct node *const add)
{
    const int index = (int) (add->hash % me->capacity);
    add->next = NULL;
    if (me->buckets[index] == NULL) {
        me->buckets[index] = add;
        return;
    }
    struct node *traverse = me->buckets[index];
    while (traverse->next != NULL) {
        traverse = traverse->next;
    }
    traverse->next = add;
}

/**
 * Rehashes all the keys in the unordered set. Used when storing references and
 * changing the keys. This should rarely be used.
 *
 * @param me The unordered set to rehash.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int unordered_set_rehash(unordered_set me)
{
    struct node **old_buckets = me->buckets;
    me->buckets = calloc((size_t) me->capacity, sizeof(struct node *));
    if (me->buckets == NULL) {
        me->buckets = old_buckets;
        return -ENOMEM;
    }
    for (int i = 0; i < me->capacity; i++) {
        struct node *traverse = old_buckets[i];
        while (traverse != NULL) {
            struct node *const backup = traverse->next;
            traverse->hash = me->hash(traverse->key);
            unordered_set_add_item(me, traverse);
            traverse = backup;
        }
    }
    free(old_buckets);
    return 0;
}

/**
 * Gets the size of the unordered set.
 *
 * @param me The unordered set to check.
 *
 * @return The size of the unordered set.
 */
int unordered_set_size(unordered_set me)
{
    return me->size;
}

/**
 * Determines whether or not the unordered set is empty.
 *
 * @param me The unordered set to check.
 *
 * @return If the unordered set is empty.
 */
bool unordered_set_is_empty(unordered_set me)
{
    return unordered_set_size(me) == 0;
}

/*
 * Increases the size of the set and redistributes the nodes.
 */
static int unordered_set_resize(unordered_set me)
{
    const int old_capacity = me->capacity;
    me->capacity *= RESIZE_RATIO;
    struct node **old_buckets = me->buckets;
    me->buckets = calloc((size_t) me->capacity, sizeof(struct node *));
    if (me->buckets == NULL) {
        me->buckets = old_buckets;
        return -ENOMEM;
    }
    for (int i = 0; i < old_capacity; i++) {
        struct node *traverse = old_buckets[i];
        while (traverse != NULL) {
            struct node *const backup = traverse->next;
            unordered_set_add_item(me, traverse);
            traverse = backup;
        }
    }
    free(old_buckets);
    return 0;
}

/*
 * Determines if an element is equal to the key.
 */
inline static bool unordered_set_is_equal(unordered_set me,
                                          const struct node *const item,
                                          const unsigned long hash,
                                          const void *const key)
{
    return item->hash == hash && me->comparator(item->key, key) == 0;
}

/*
 * Creates an element to add.
 */
static struct node *const unordered_set_create_element(unordered_set me,
                                                       const unsigned long hash,
                                                       const void *const key)
{
    struct node *const init = malloc(sizeof(struct node));
    if (init == NULL) {
        return NULL;
    }
    init->key = malloc(me->key_size);
    if (init->key == NULL) {
        free(init);
        return NULL;
    }
    memcpy(init->key, key, me->key_size);
    init->hash = hash;
    init->next = NULL;
    return init;
}

/**
 * Adds an element to the unordered set if the unordered set does not already
 * contain it.
 *
 * @param me  The unordered set to add to.
 * @param key The element to add.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int unordered_set_put(unordered_set me, void *const key)
{

    const unsigned long hash = me->hash(key);
    const int index = (int) (hash % me->capacity);
    if (me->buckets[index] == NULL) {
        me->buckets[index] = unordered_set_create_element(me, hash, key);
        if (me->buckets[index] == NULL) {
            return -ENOMEM;
        }
    } else {
        struct node *traverse = me->buckets[index];
        if (unordered_set_is_equal(me, traverse, hash, key)) {
            return 0;
        }
        while (traverse->next != NULL) {
            traverse = traverse->next;
            if (unordered_set_is_equal(me, traverse, hash, key)) {
                return 0;
            }
        }
        traverse->next = unordered_set_create_element(me, hash, key);
        if (traverse->next == NULL) {
            return -ENOMEM;
        }
    }
    me->size++;
    if (me->size >= RESIZE_AT * me->capacity) {
        return unordered_set_resize(me);
    }
    return 0;
}

/**
 * Determines if the unordered set contains the specified element.
 *
 * @param me  The unordered set to check for the element.
 * @param key The element to check.
 *
 * @return If the unordered set contained the element.
 */
bool unordered_set_contains(unordered_set me, void *const key)
{
    const unsigned long hash = me->hash(key);
    const int index = (int) (hash % me->capacity);
    const struct node *traverse = me->buckets[index];
    while (traverse != NULL) {
        if (unordered_set_is_equal(me, traverse, hash, key)) {
            return true;
        }
        traverse = traverse->next;
    }
    return false;
}

/**
 * Removes the element from the unordered set if it contains it.
 *
 * @param me  The unordered set to remove an element from.
 * @param key The element to remove.
 *
 * @return If the unordered set contained the element.
 */
bool unordered_set_remove(unordered_set me, void *const key)
{
    const unsigned long hash = me->hash(key);
    const int index = (int) (hash % me->capacity);
    if (me->buckets[index] == NULL) {
        return false;
    }
    struct node *traverse = me->buckets[index];
    if (unordered_set_is_equal(me, traverse, hash, key)) {
        me->buckets[index] = traverse->next;
        free(traverse->key);
        free(traverse);
        me->size--;
        return true;
    }
    while (traverse->next != NULL) {
        if (unordered_set_is_equal(me, traverse->next, hash, key)) {
            struct node *const backup = traverse->next;
            traverse->next = traverse->next->next;
            free(backup->key);
            free(backup);
            me->size--;
            return true;
        }
        traverse = traverse->next;
    }
    return false;
}

/**
 * Clears the elements from the unordered set.
 *
 * @param me The unordered set to clear.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int unordered_set_clear(unordered_set me)
{
    for (int i = 0; i < me->capacity; i++) {
        struct node *traverse = me->buckets[i];
        while (traverse != NULL) {
            struct node *const backup = traverse;
            traverse = traverse->next;
            free(backup->key);
            free(backup);
        }
        me->buckets[i] = NULL;
    }
    me->size = 0;
    me->capacity = STARTING_BUCKETS;
    struct node **temp = calloc((size_t) me->capacity, sizeof(struct node *));
    if (temp == NULL) {
        return -ENOMEM;
    }
    free(me->buckets);
    me->buckets = temp;
    return 0;
}

/**
 * Frees the unordered set memory.
 *
 * @param me The unordered set to free from memory.
 *
 * @return NULL
 */
unordered_set unordered_set_destroy(unordered_set me)
{
    unordered_set_clear(me);
    free(me->buckets);
    free(me);
    return NULL;
}
