/*
 * Copyright (c) 2017-2020 Bailey Thompson
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
#include "include/unordered_set.h"

static const int STARTING_BUCKETS = 16;
static const double RESIZE_AT = 0.75;
static const double RESIZE_RATIO = 2;

struct internal_unordered_set {
    size_t key_size;
    size_t size;
    size_t capacity;
    unsigned long (*hash)(const void *const key);
    int (*comparator)(const void *const one, const void *const two);
    char **buckets;
};

static const size_t node_next_offset = 0;
static const size_t node_hash_offset = sizeof(char *);
static const size_t node_key_offset = sizeof(char *) + sizeof(unsigned long);

/*
 * Gets the hash by first calling the user-defined hash, and then using a
 * second hash to prevent hashing clusters if the user-defined hash is
 * sub-optimal.
 */
static unsigned long unordered_set_hash(unordered_set me, const void *const key)
{
    unsigned long hash = me->hash(key);
    hash ^= (hash >> 20UL) ^ (hash >> 12UL);
    return hash ^ (hash >> 7UL) ^ (hash >> 4UL);
}

/**
 * Initializes an unordered set.
 *
 * @param key_size   the size of each key in the unordered set; must be
 *                   positive
 * @param hash       the hash function which computes the hash from the key;
 *                   must not be NULL
 * @param comparator the comparator function which compares two keys; must not
 *                   be NULL
 *
 * @return the newly-initialized unordered set, or NULL if it was not
 *         successfully initialized due to either invalid input arguments or
 *         memory allocation error
 */
unordered_set unordered_set_init(const size_t key_size,
                                 unsigned long (*hash)(const void *const),
                                 int (*comparator)(const void *const,
                                                   const void *const))
{
    struct internal_unordered_set *init;
    if (key_size == 0 || !hash || !comparator) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_unordered_set));
    if (!init) {
        return NULL;
    }
    init->key_size = key_size;
    init->hash = hash;
    init->comparator = comparator;
    init->size = 0;
    init->capacity = STARTING_BUCKETS;
    init->buckets = calloc(STARTING_BUCKETS, sizeof(struct node *));
    if (!init->buckets) {
        free(init);
        return NULL;
    }
    return init;
}

/*
 * Adds the specified node to the set.
 */
static void unordered_set_add_item(unordered_set me, char *const add)
{
    char *traverse;
    char *traverse_next;
    unsigned long hash;
    size_t index;
    memcpy(&hash, add + node_hash_offset, sizeof(unsigned long));
    index = hash % me->capacity;
    memset(add + node_next_offset, 0, sizeof(char *));
    if (!me->buckets[index]) {
        me->buckets[index] = add;
        return;
    }
    traverse = me->buckets[index];
    memcpy(&traverse_next, traverse + node_next_offset, sizeof(char *));
    while (traverse_next) {
        traverse = traverse_next;
        memcpy(&traverse_next, traverse + node_next_offset, sizeof(char *));
    }
    memcpy(traverse + node_next_offset, &add, sizeof(char *));
}

/**
 * Rehashes all the keys in the unordered set. Used when storing references and
 * changing the keys. This should rarely be used.
 *
 * @param me the unordered set to rehash
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_set_rehash(unordered_set me)
{
    size_t i;
    char **old_buckets = me->buckets;
    me->buckets = calloc(me->capacity, sizeof(char *));
    if (!me->buckets) {
        me->buckets = old_buckets;
        return -ENOMEM;
    }
    for (i = 0; i < me->capacity; i++) {
        char *traverse = old_buckets[i];
        while (traverse) {
            char *backup;
            char *key;
            unsigned long hash;
            memcpy(&backup, traverse + node_next_offset, sizeof(char *));
            key = traverse + node_key_offset;
            hash = unordered_set_hash(me, key);
            memcpy(traverse + node_hash_offset, &hash, sizeof(unsigned long));
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
 * @param me the unordered set to check
 *
 * @return the size of the unordered set
 */
size_t unordered_set_size(unordered_set me)
{
    return me->size;
}

/**
 * Determines whether or not the unordered set is empty.
 *
 * @param me the unordered set to check
 *
 * @return 1 if the unordered set is empty, otherwise 0
 */
int unordered_set_is_empty(unordered_set me)
{
    return unordered_set_size(me) == 0;
}

/*
 * Increases the size of the set and redistributes the nodes.
 */
static int unordered_set_resize(unordered_set me)
{
    size_t i;
    const size_t old_capacity = me->capacity;
    const size_t new_capacity = (size_t) (me->capacity * RESIZE_RATIO);
    char **old_buckets = me->buckets;
    me->buckets = calloc(new_capacity, sizeof(char *));
    if (!me->buckets) {
        me->buckets = old_buckets;
        return -ENOMEM;
    }
    me->capacity = new_capacity;
    for (i = 0; i < old_capacity; i++) {
        char *traverse = old_buckets[i];
        while (traverse) {
            char *backup;
            memcpy(&backup, traverse + node_next_offset, sizeof(char *));
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
static int unordered_set_is_equal(unordered_set me, char *const item,
                                  const unsigned long hash,
                                  const void *const key)
{
    unsigned long item_hash;
    memcpy(&item_hash, item + node_hash_offset, sizeof(unsigned long));
    return item_hash == hash &&
           me->comparator(item + node_key_offset, key) == 0;
}

/*
 * Creates an element to add.
 */
static char *unordered_set_create_element(unordered_set me,
                                          const unsigned long hash,
                                          const void *const key)
{
    char *init = malloc(sizeof(char *) + sizeof(unsigned long) + me->key_size);
    if (!init) {
        return NULL;
    }
    memset(init + node_next_offset, 0, sizeof(char *));
    memcpy(init + node_hash_offset, &hash, sizeof(unsigned long));
    memcpy(init + node_key_offset, key, me->key_size);
    return init;
}

/**
 * Adds an element to the unordered set if the unordered set does not already
 * contain it. The pointer to the key being passed in should point to the key
 * type which this unordered set holds. For example, if this unordered set holds
 * key integers, the key pointer should be a pointer to an integer. Since the
 * key is being copied, the pointer only has to be valid when this function is
 * called.
 *
 * @param me  the unordered set to add to
 * @param key the element to add
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_set_put(unordered_set me, void *const key)
{
    const unsigned long hash = unordered_set_hash(me, key);
    int index;
    if (me->size + 1 >= (size_t) (RESIZE_AT * me->capacity)) {
        const int rc = unordered_set_resize(me);
        if (rc != 0) {
            return rc;
        }
    }
    index = (size_t) (hash % me->capacity);
    if (!me->buckets[index]) {
        me->buckets[index] = unordered_set_create_element(me, hash, key);
        if (!me->buckets[index]) {
            return -ENOMEM;
        }
    } else {
        char *traverse = me->buckets[index];
        char *traverse_next;
        if (unordered_set_is_equal(me, traverse, hash, key)) {
            return 0;
        }
        memcpy(&traverse_next, traverse + node_next_offset, sizeof(char *));
        while (traverse_next) {
            traverse = traverse_next;
            memcpy(&traverse_next, traverse + node_next_offset, sizeof(char *));
            if (unordered_set_is_equal(me, traverse, hash, key)) {
                return 0;
            }
        }
        traverse_next = unordered_set_create_element(me, hash, key);
        if (!traverse_next) {
            return -ENOMEM;
        }
        memcpy(traverse + node_next_offset, &traverse_next, sizeof(char *));
    }
    me->size++;
    return 0;
}

/**
 * Determines if the unordered set contains the specified element. The pointer
 * to the key being passed in should point to the key type which this unordered
 * set holds. For example, if this unordered set holds key integers, the key
 * pointer should be a pointer to an integer. Since the key is being copied,
 * the pointer only has to be valid when this function is called.
 *
 * @param me  the unordered set to check for the element
 * @param key the element to check
 *
 * @return 1 if the unordered set contained the element, otherwise 0
 */
int unordered_set_contains(unordered_set me, void *const key)
{
    const unsigned long hash = unordered_set_hash(me, key);
    char *traverse = me->buckets[hash % me->capacity];
    while (traverse) {
        if (unordered_set_is_equal(me, traverse, hash, key)) {
            return 1;
        }
        memcpy(&traverse, traverse + node_next_offset, sizeof(char *));
    }
    return 0;
}

/**
 * Removes the key from the unordered set if it contains it. The pointer to the
 * key being passed in should point to the key type which this unordered set
 * holds. For example, if this unordered set holds key integers, the key pointer
 * should be a pointer to an integer. Since the key is being copied, the pointer
 * only has to be valid when this function is called.
 *
 * @param me  the unordered set to remove a key from
 * @param key the key to remove
 *
 * @return 1 if the unordered set contained the key, otherwise 0
 */
int unordered_set_remove(unordered_set me, void *const key)
{
    char *traverse;
    char *traverse_next;
    const unsigned long hash = unordered_set_hash(me, key);
    const size_t index = hash % me->capacity;
    if (!me->buckets[index]) {
        return 0;
    }
    traverse = me->buckets[index];
    if (unordered_set_is_equal(me, traverse, hash, key)) {
        memcpy(&traverse_next, traverse + node_next_offset, sizeof(char *));
        me->buckets[index] = traverse_next;
        free(traverse);
        me->size--;
        return 1;
    }
    memcpy(&traverse_next, traverse + node_next_offset, sizeof(char *));
    while (traverse_next) {
        if (unordered_set_is_equal(me, traverse_next, hash, key)) {
            char *backup = traverse_next;
            char *backup_next;
            memcpy(&backup_next, backup + node_next_offset, sizeof(char *));
            memcpy(traverse + node_next_offset, &backup_next, sizeof(char *));
            free(backup);
            me->size--;
            return 1;
        }
        traverse = traverse_next;
        memcpy(&traverse_next, traverse + node_next_offset, sizeof(char *));
    }
    return 0;
}

/**
 * Clears the keys from the unordered set.
 *
 * @param me the unordered set to clear
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_set_clear(unordered_set me)
{
    size_t i;
    char **updated_buckets = calloc(STARTING_BUCKETS, sizeof(char *));
    if (!updated_buckets) {
        return -ENOMEM;
    }
    for (i = 0; i < me->capacity; i++) {
        char *traverse = me->buckets[i];
        while (traverse) {
            char *backup = traverse;
            memcpy(&traverse, traverse + node_next_offset, sizeof(char *));
            free(backup);
        }
    }
    me->size = 0;
    me->capacity = STARTING_BUCKETS;
    me->buckets = updated_buckets;
    return 0;
}

/**
 * Frees the unordered set memory. Performing further operations after calling
 * this function results in undefined behavior.
 *
 * @param me the unordered set to free from memory
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
