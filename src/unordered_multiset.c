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
#include "include/unordered_multiset.h"

#define STARTING_BUCKETS 16
#define RESIZE_AT 0.75
#define RESIZE_RATIO 2

struct internal_unordered_multiset {
    size_t key_size;
    size_t size;
    size_t capacity;
    size_t used;
    unsigned long (*hash)(const void *const key);
    int (*comparator)(const void *const one, const void *const two);
    char **buckets;
};

static const size_t ptr_size = sizeof(char *);
static const size_t count_size = sizeof(size_t);
static const size_t hash_size = sizeof(unsigned long);
static const size_t node_next_offset = 0;
static const size_t node_count_offset = sizeof(char *);
static const size_t node_hash_offset = sizeof(char *) + sizeof(size_t);
static const size_t node_key_offset =
        sizeof(char *) + sizeof(size_t) + sizeof(unsigned long);

/*
 * Gets the hash by first calling the user-defined hash, and then using a
 * second hash to prevent hashing clusters if the user-defined hash is
 * sub-optimal.
 */
static unsigned long unordered_multiset_hash(unordered_multiset me,
                                             const void *const key)
{
    unsigned long hash = me->hash(key);
    hash ^= (hash >> 20UL) ^ (hash >> 12UL);
    return hash ^ (hash >> 7UL) ^ (hash >> 4UL);
}

/**
 * Initializes an unordered multi-set.
 *
 * @param key_size   the size of each key in the unordered multi-set; must be
 *                   positive
 * @param hash       the hash function which computes the hash from the key;
 *                   must not be NULL
 * @param comparator the comparator function which compares two keys; must not
 *                   be NULL
 *
 * @return the newly-initialized unordered multi-set, or NULL if it was not
 *         successfully initialized due to either invalid input arguments or
 *         memory allocation error
 */
unordered_multiset
unordered_multiset_init(const size_t key_size,
                        unsigned long (*hash)(const void *const),
                        int (*comparator)(const void *const, const void *const))
{
    struct internal_unordered_multiset *init;
    if (key_size == 0 || !hash || !comparator) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_unordered_multiset));
    if (!init) {
        return NULL;
    }
    init->key_size = key_size;
    init->hash = hash;
    init->comparator = comparator;
    init->size = 0;
    init->capacity = STARTING_BUCKETS;
    init->used = 0;
    init->buckets = calloc(STARTING_BUCKETS, ptr_size);
    if (!init->buckets) {
        free(init);
        return NULL;
    }
    return init;
}

/*
 * Adds the specified node to the multi-set.
 */
static void unordered_multiset_add_item(unordered_multiset me, char *const add)
{
    char *traverse;
    char *traverse_next;
    unsigned long hash;
    size_t index;
    memcpy(&hash, add + node_hash_offset, hash_size);
    index = hash % me->capacity;
    memset(add + node_next_offset, 0, ptr_size);
    if (!me->buckets[index]) {
        me->buckets[index] = add;
        return;
    }
    traverse = me->buckets[index];
    memcpy(&traverse_next, traverse + node_next_offset, ptr_size);
    while (traverse_next) {
        traverse = traverse_next;
        memcpy(&traverse_next, traverse + node_next_offset, ptr_size);
    }
    memcpy(traverse + node_next_offset, &add, ptr_size);
}

/**
 * Rehashes all the keys in the unordered multi-set. Used when storing
 * references and changing the keys. This should rarely be used.
 *
 * @param me the unordered multi-set to rehash
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_multiset_rehash(unordered_multiset me)
{
    size_t i;
    char **old_buckets = me->buckets;
    me->buckets = calloc(me->capacity, ptr_size);
    if (!me->buckets) {
        me->buckets = old_buckets;
        return -ENOMEM;
    }
    for (i = 0; i < me->capacity; i++) {
        char *traverse = old_buckets[i];
        while (traverse) {
            char *backup;
            unsigned long hash;
            memcpy(&backup, traverse + node_next_offset, ptr_size);
            hash = unordered_multiset_hash(me, traverse + node_key_offset);
            memcpy(traverse + node_hash_offset, &hash, hash_size);
            unordered_multiset_add_item(me, traverse);
            traverse = backup;
        }
    }
    free(old_buckets);
    return 0;
}

/**
 * Gets the size of the unordered multi-set.
 *
 * @param me the unordered multi-set to check
 *
 * @return the size of the unordered multi-set
 */
size_t unordered_multiset_size(unordered_multiset me)
{
    return me->size;
}

/**
 * Determines whether or not the unordered multi-set is empty.
 *
 * @param me the unordered multi-set to check
 *
 * @return 1 if the unordered multi-set is empty, otherwise 0
 */
int unordered_multiset_is_empty(unordered_multiset me)
{
    return unordered_multiset_size(me) == 0;
}

/*
 * Increases the size of the multi-set and redistributes the nodes.
 */
static int unordered_multiset_resize(unordered_multiset me)
{
    size_t i;
    const size_t old_capacity = me->capacity;
    const size_t new_capacity = me->capacity * RESIZE_RATIO;
    char **old_buckets = me->buckets;
    me->buckets = calloc(new_capacity, ptr_size);
    if (!me->buckets) {
        me->buckets = old_buckets;
        return -ENOMEM;
    }
    me->capacity = new_capacity;
    for (i = 0; i < old_capacity; i++) {
        char *traverse = old_buckets[i];
        while (traverse) {
            char *backup;
            memcpy(&backup, traverse + node_next_offset, ptr_size);
            unordered_multiset_add_item(me, traverse);
            traverse = backup;
        }
    }
    free(old_buckets);
    return 0;
}

/*
 * Determines if an element is equal to the key.
 */
static int unordered_multiset_is_equal(unordered_multiset me, char *const item,
                                       const unsigned long hash,
                                       const void *const key)
{
    unsigned long item_hash;
    memcpy(&item_hash, item + node_hash_offset, hash_size);
    return item_hash == hash &&
           me->comparator(item + node_key_offset, key) == 0;
}

/*
 * Creates an element to add.
 */
static char *unordered_multiset_create_element(unordered_multiset me,
                                               const unsigned long hash,
                                               const void *const key)
{
    const size_t one = 1;
    char *init = malloc(ptr_size + count_size + hash_size + me->key_size);
    if (!init) {
        return NULL;
    }
    memset(init + node_next_offset, 0, ptr_size);
    memcpy(init + node_count_offset, &one, count_size);
    memcpy(init + node_hash_offset, &hash, hash_size);
    memcpy(init + node_key_offset, key, me->key_size);
    return init;
}

/**
 * Adds an element to the unordered multi-set. The pointer to the key being
 * passed in should point to the key type which this unordered multi-set holds.
 * For example, if this multi-set holds key integers, the key pointer should be
 * a pointer to an integer. Since the key is being copied, the pointer only has
 * to be valid when this function is called.
 *
 * @param me  the unordered multi-set to add to
 * @param key the element to add
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_multiset_put(unordered_multiset me, void *const key)
{
    const unsigned long hash = unordered_multiset_hash(me, key);
    int index;
    if (me->used + 1 >= (size_t) (RESIZE_AT * me->capacity)) {
        const int rc = unordered_multiset_resize(me);
        if (rc != 0) {
            return rc;
        }
    }
    index = (size_t) (hash % me->capacity);
    if (!me->buckets[index]) {
        me->buckets[index] = unordered_multiset_create_element(me, hash, key);
        if (!me->buckets[index]) {
            return -ENOMEM;
        }
    } else {
        char *traverse = me->buckets[index];
        char *traverse_next;
        if (unordered_multiset_is_equal(me, traverse, hash, key)) {
            size_t count;
            memcpy(&count, traverse + node_count_offset, count_size);
            count++;
            memcpy(traverse + node_count_offset, &count, count_size);
            me->size++;
            return 0;
        }
        memcpy(&traverse_next, traverse + node_next_offset, ptr_size);
        while (traverse_next) {
            traverse = traverse_next;
            memcpy(&traverse_next, traverse + node_next_offset, ptr_size);
            if (unordered_multiset_is_equal(me, traverse, hash, key)) {
                size_t count;
                memcpy(&count, traverse + node_count_offset, count_size);
                count++;
                memcpy(traverse + node_count_offset, &count, count_size);
                me->size++;
                return 0;
            }
        }
        traverse_next = unordered_multiset_create_element(me, hash, key);
        if (!traverse_next) {
            return -ENOMEM;
        }
        memcpy(traverse + node_next_offset, &traverse_next, ptr_size);
    }
    me->size++;
    me->used++;
    return 0;
}

/**
 * Determines the count of a specific key in the unordered multi-set. The
 * pointer to the key being passed in should point to the key type which this
 * unordered multi-set holds. For example, if this unordered multi-set holds key
 * integers, the key pointer should be a pointer to an integer. Since the key is
 * being copied, the pointer only has to be valid when this function is called.
 *
 * @param me  the unordered multi-set to check for the count
 * @param key the element to check
 *
 * @return the count of a specific key in the unordered multi-set
 */
size_t unordered_multiset_count(unordered_multiset me, void *const key)
{
    const unsigned long hash = unordered_multiset_hash(me, key);
    char *traverse = me->buckets[hash % me->capacity];
    while (traverse) {
        if (unordered_multiset_is_equal(me, traverse, hash, key)) {
            size_t count;
            memcpy(&count, traverse + node_count_offset, count_size);
            return count;
        }
        memcpy(&traverse, traverse + node_next_offset, ptr_size);
    }
    return 0;
}

/**
 * Determines if the unordered multi-set contains the specified element. The
 * pointer to the key being passed in should point to the key type which this
 * unordered multi-set holds. For example, if this unordered multi-set holds key
 * integers, the key pointer should be a pointer to an integer. Since the key is
 * being copied, the pointer only has to be valid when this function is called.
 *
 * @param me  the unordered multi-set to check for the key
 * @param key the key to check
 *
 * @return 1 if the unordered multi-set contained the key, otherwise 0
 */
int unordered_multiset_contains(unordered_multiset me, void *const key)
{
    return unordered_multiset_count(me, key) != 0;
}

/**
 * Removes a key from the unordered multi-set if it contains it. The pointer to
 * the key being passed in should point to the key type which this unordered
 * multi-set holds. For example, if this unordered multi-set holds key integers,
 * the key pointer should be a pointer to an integer. Since the key is being
 * copied, the pointer only has to be valid when this function is called.
 *
 * @param me  the unordered multi-set to remove a key from
 * @param key the key to remove
 *
 * @return 1 if the unordered multi-set contained the key, otherwise 0
 */
int unordered_multiset_remove(unordered_multiset me, void *const key)
{
    char *traverse;
    char *traverse_next;
    const unsigned long hash = unordered_multiset_hash(me, key);
    const size_t index = hash % me->capacity;
    if (!me->buckets[index]) {
        return 0;
    }
    traverse = me->buckets[index];
    if (unordered_multiset_is_equal(me, traverse, hash, key)) {
        size_t count;
        memcpy(&count, traverse + node_count_offset, count_size);
        if (count == 1) {
            memcpy(me->buckets + index, traverse + node_next_offset, ptr_size);
            free(traverse);
            me->used--;
        } else {
            count--;
            memcpy(traverse + node_count_offset, &count, count_size);
        }
        me->size--;
        return 1;
    }
    memcpy(&traverse_next, traverse + node_next_offset, ptr_size);
    while (traverse_next) {
        if (unordered_multiset_is_equal(me, traverse_next, hash, key)) {
            size_t count;
            memcpy(&count, traverse_next + node_count_offset, count_size);
            if (count == 1) {
                memcpy(traverse + node_next_offset,
                       traverse_next + node_next_offset, ptr_size);
                free(traverse_next);
                me->used--;
            } else {
                count--;
                memcpy(traverse_next + node_count_offset, &count, count_size);
            }
            me->size--;
            return 1;
        }
        traverse = traverse_next;
        memcpy(&traverse_next, traverse + node_next_offset, ptr_size);
    }
    return 0;
}

/**
 * Removes all the keys specified by the key from an unordered multi-set if it
 * contains the key. The pointer to the key being passed in should point to the
 * key type which this multi-set holds. For example, if this multi-set holds key
 * integers, the key pointer should be a pointer to an integer. Since the key is
 * being copied, the pointer only has to be valid when this function is called.
 *
 * @param me  the unordered multi-set to remove a key from
 * @param key the key to remove
 *
 * @return 1 if the unordered multi-set contained the key, otherwise 0
 */
int unordered_multiset_remove_all(unordered_multiset me, void *const key)
{
    char *traverse;
    char *traverse_next;
    const unsigned long hash = unordered_multiset_hash(me, key);
    const size_t index = hash % me->capacity;
    if (!me->buckets[index]) {
        return 0;
    }
    traverse = me->buckets[index];
    if (unordered_multiset_is_equal(me, traverse, hash, key)) {
        size_t count;
        memcpy(&count, traverse + node_count_offset, count_size);
        memcpy(me->buckets + index, traverse + node_next_offset, ptr_size);
        free(traverse);
        me->used--;
        me->size -= count;
        return 1;
    }
    memcpy(&traverse_next, traverse + node_next_offset, ptr_size);
    while (traverse_next) {
        if (unordered_multiset_is_equal(me, traverse_next, hash, key)) {
            size_t count;
            memcpy(&count, traverse_next + node_count_offset, count_size);
            memcpy(traverse + node_next_offset,
                   traverse_next + node_next_offset, ptr_size);
            free(traverse_next);
            me->used--;
            me->size -= count;
            return 1;
        }
        traverse = traverse_next;
        memcpy(&traverse_next, traverse + node_next_offset, ptr_size);
    }
    return 0;
}

/**
 * Clears the keys from the unordered multi-set.
 *
 * @param me the unordered multi-set to clear
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_multiset_clear(unordered_multiset me)
{
    size_t i;
    char **updated_buckets = calloc(STARTING_BUCKETS, ptr_size);
    if (!updated_buckets) {
        return -ENOMEM;
    }
    for (i = 0; i < me->capacity; i++) {
        char *traverse = me->buckets[i];
        while (traverse) {
            char *backup = traverse;
            memcpy(&traverse, traverse + node_next_offset, ptr_size);
            free(backup);
        }
    }
    me->size = 0;
    me->capacity = STARTING_BUCKETS;
    me->used = 0;
    me->buckets = updated_buckets;
    return 0;
}

/**
 * Frees the unordered multi-set memory. Performing further operations after
 * calling this function results in undefined behavior.
 *
 * @param me the unordered multi-set to free from memory
 *
 * @return NULL
 */
unordered_multiset unordered_multiset_destroy(unordered_multiset me)
{
    unordered_multiset_clear(me);
    free(me->buckets);
    free(me);
    return NULL;
}
