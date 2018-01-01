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
#include "unordered_multimap.h"

static const int STARTING_BUCKETS = 8;
static const double RESIZE_AT = 0.75;
static const double RESIZE_RATIO = 1.5;

struct _unordered_multimap {
    size_t key_size;
    size_t value_size;
    unsigned long (*hash)(const void *const key);
    int (*key_comparator)(const void *const one, const void *const two);
    int (*value_comparator)(const void *const one, const void *const two);
    int size;
    int capacity;
    struct node **buckets;
    unsigned long iterate_hash;
    void *iterate_key;
    struct node *iterate_element;
};

struct node {
    void *key;
    void *value;
    unsigned long hash;
    struct node *next;
};

/**
 * Initializes an unordered multi-map, which is a collection of key-value pairs,
 * hashed by keys
 *
 * @param key_size         The size of each key in the unordered multi-map.
 * @param value_size       The size of each value in the unordered multi-map.
 * @param hash             The hash function which computes the hash from key.
 * @param key_comparator   The comparator function which compares two keys.
 * @param value_comparator The comparator function which compares two values.
 *
 * @return The newly-initialized unordered multi-map, or NULL if memory
 *         allocation error.
 */
unordered_multimap
unordered_multimap_init(const size_t key_size,
                        const size_t value_size,
                        unsigned long (*hash)(const void *const),
                        int (*key_comparator)(const void *const,
                                              const void *const),
                        int (*value_comparator)(const void *const,
                                                const void *const))
{
    struct _unordered_multimap *const init =
            malloc(sizeof(struct _unordered_multimap));
    if (init == NULL) {
        return NULL;
    }
    init->key_size = key_size;
    init->value_size = value_size;
    init->hash = hash;
    init->key_comparator = key_comparator;
    init->value_comparator = value_comparator;
    init->size = 0;
    init->capacity = STARTING_BUCKETS;
    init->buckets = calloc(STARTING_BUCKETS, sizeof(struct node *));
    if (init->buckets == NULL) {
        free(init);
        return NULL;
    }
    init->iterate_hash = 0;
    init->iterate_key = calloc(1, init->key_size);
    if (init->iterate_key == NULL) {
        free(init->buckets);
        free(init);
        return NULL;
    }
    init->iterate_element = NULL;
    return init;
}

/*
 * Adds the specified node to the multi-map.
 */
static void unordered_multimap_add_item(unordered_multimap me,
                                        struct node *const add)
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
 * Rehashes all the keys in the unordered multi-map. Used when storing
 * references and changing the keys. This should rarely be used.
 *
 * @param me The unordered multi-map to rehash.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int unordered_multimap_rehash(unordered_multimap me)
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
            unordered_multimap_add_item(me, traverse);
            traverse = backup;
        }
    }
    free(old_buckets);
    return 0;
}

/**
 * Gets the size of the unordered multi-map.
 *
 * @param me The unordered multi-map to check.
 *
 * @return The size of the unordered multi-map.
 */
int unordered_multimap_size(unordered_multimap me)
{
    return me->size;
}

/**
 * Determines whether or not the unordered multi-map is empty.
 *
 * @param me The unordered multi-map to check.
 *
 * @return If the unordered multi-map is empty.
 */
bool unordered_multimap_is_empty(unordered_multimap me)
{
    return unordered_multimap_size(me) == 0;
}

/*
 * Increases the size of the multi-map and redistributes the nodes.
 */
static int unordered_multimap_resize(unordered_multimap me)
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
            unordered_multimap_add_item(me, traverse);
            traverse = backup;
        }
    }
    free(old_buckets);
    return 0;
}

/*
 * Determines if an element is equal to the key.
 */
inline static bool unordered_multimap_is_equal(unordered_multimap me,
                                               const struct node *const item,
                                               const unsigned long hash,
                                               const void *const key)
{
    return item->hash == hash && me->key_comparator(item->key, key) == 0;
}

/*
 * Creates an element to add.
 */
static struct node *const
unordered_multimap_create_element(unordered_multimap me,
                                  const unsigned long hash,
                                  const void *const key,
                                  const void *const value)
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
    init->value = malloc(me->value_size);
    if (init->value == NULL) {
        free(init->key);
        free(init);
        return NULL;
    }
    memcpy(init->value, value, me->value_size);
    init->hash = hash;
    init->next = NULL;
    return init;
}

/**
 * Adds a key-value pair to the unordered multi-map.
 *
 * @param me    The unordered multi-map to add to.
 * @param key   The key to add.
 * @param value The value to add.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int unordered_multimap_put(unordered_multimap me,
                           void *const key,
                           void *const value)
{
    const unsigned long hash = me->hash(key);
    const int index = (int) (hash % me->capacity);
    if (me->buckets[index] == NULL) {
        me->buckets[index] =
                unordered_multimap_create_element(me, hash, key, value);
        if (me->buckets[index] == NULL) {
            return -ENOMEM;
        }
    } else {
        struct node *traverse = me->buckets[index];
        while (traverse->next != NULL) {
            traverse = traverse->next;
        }
        traverse->next =
                unordered_multimap_create_element(me, hash, key, value);
        if (traverse->next == NULL) {
            return -ENOMEM;
        }
    }
    me->size++;
    if (me->size >= RESIZE_AT * me->capacity) {
        return unordered_multimap_resize(me);
    }
    return 0;
}

/**
 * Creates the iterator for the specified key. To iterate over the values, keep
 * getting the next value. Between starting and iterations, the unordered
 * multi-map must not be mutated.
 *
 * @param me  The unordered multi-map to start the iterator for.
 * @param key The key to start the iterator for.
 */
void unordered_multimap_get_start(unordered_multimap me, void *const key)
{
    me->iterate_hash = me->hash(key);
    memcpy(me->iterate_key, key, me->key_size);
    me->iterate_element = NULL;
    const int index = (int) (me->iterate_hash % me->capacity);
    struct node *traverse = me->buckets[index];
    while (traverse != NULL) {
        if (unordered_multimap_is_equal(me, traverse, me->iterate_hash, key)) {
            me->iterate_element = traverse;
            return;
        }
        traverse = traverse->next;
    }
}

/**
 * Iterates over the values for the specified key. Must be called after starting
 * the iterator. The unordered multi-map must not be mutated between start and
 * iterations.
 *
 * @param value The value to be copied to from iteration.
 * @param me    The unordered multi-map to iterate over.
 *
 * @return If there exist no more values for the key which is being iterated
 *         over.
 */
bool unordered_multimap_get_next(void *const value, unordered_multimap me)
{
    if (me->iterate_element == NULL) {
        return false;
    }
    struct node *const item = me->iterate_element;
    struct node *traverse = item->next;
    while (traverse != NULL) {
        if (unordered_multimap_is_equal(me, traverse, me->iterate_hash,
                                        me->iterate_key)) {
            me->iterate_element = traverse;
            memcpy(value, item->value, me->value_size);
            return true;
        }
        traverse = traverse->next;
    }
    me->iterate_element = NULL;
    memcpy(value, item->value, me->value_size);
    return true;
}

/**
 * Determines the amount of times the key appears in the unordered multi-map.
 *
 * @param me  The unordered multi-map to check for the key.
 * @param key The key to check.
 *
 * @return The amount of times the key appears in the unordered multi-map.
 */
int unordered_multimap_count(unordered_multimap me, void *const key)
{
    int count = 0;
    const unsigned long hash = me->hash(key);
    const int index = (int) (hash % me->capacity);
    const struct node *traverse = me->buckets[index];
    while (traverse != NULL) {
        if (unordered_multimap_is_equal(me, traverse, hash, key)) {
            count++;
        }
        traverse = traverse->next;
    }
    return count;
}

/**
 * Determines if the unordered multi-map contains the specified key.
 *
 * @param me  The unordered multi-map to check for the key.
 * @param key The key to check.
 *
 * @return If the unordered multi-map contained the key.
 */
bool unordered_multimap_contains(unordered_multimap me, void *const key)
{
    const unsigned long hash = me->hash(key);
    const int index = (int) (hash % me->capacity);
    const struct node *traverse = me->buckets[index];
    while (traverse != NULL) {
        if (unordered_multimap_is_equal(me, traverse, hash, key)) {
            return true;
        }
        traverse = traverse->next;
    }
    return false;
}

/**
 * Removes the key-value pair from the unordered multi-map if it contains it.
 *
 * @param me    The unordered multi-map to remove an key from.
 * @param key   The key to remove.
 * @param value The value to remove.
 *
 * @return If the unordered multi-map contained the key.
 */
bool unordered_multimap_remove(unordered_multimap me,
                               void *const key,
                               void *const value)
{
    const unsigned long hash = me->hash(key);
    const int index = (int) (hash % me->capacity);
    if (me->buckets[index] == NULL) {
        return false;
    }
    struct node *traverse = me->buckets[index];
    bool is_key_equal = unordered_multimap_is_equal(me, traverse, hash, key);
    if (is_key_equal && me->value_comparator(traverse->value, value) == 0) {
        me->buckets[index] = traverse->next;
        free(traverse->key);
        free(traverse->value);
        free(traverse);
        me->size--;
        return true;
    }
    while (traverse->next != NULL) {
        is_key_equal =
                unordered_multimap_is_equal(me, traverse->next, hash, key);
        if (is_key_equal && me->value_comparator(traverse->value, value) == 0) {
            struct node *const backup = traverse->next;
            traverse->next = traverse->next->next;
            free(backup->key);
            free(backup->value);
            free(backup);
            me->size--;
            return true;
        }
        traverse = traverse->next;
    }
    return false;
}

/**
 * Removes all the key-value pairs from the unordered multi-map specified by the
 * key.
 *
 * @param me  The unordered multi-map to remove a key-value pair from.
 * @param key The key to remove.
 *
 * @return If the unordered multi-map contained the key.
 */
bool unordered_multimap_remove_all(unordered_multimap me, void *const key)
{
    const unsigned long hash = me->hash(key);
    const int index = (int) (hash % me->capacity);
    bool was_modified = false;
    while (true) {
        struct node *traverse = me->buckets[index];
        if (traverse == NULL) {
            break;
        }
        if (unordered_multimap_is_equal(me, traverse, hash, key)) {
            me->buckets[index] = traverse->next;
            free(traverse->key);
            free(traverse->value);
            free(traverse);
            me->size--;
            was_modified = true;
            continue;
        }
        while (traverse->next != NULL) {
            if (unordered_multimap_is_equal(me, traverse->next, hash, key)) {
                struct node *const backup = traverse->next;
                traverse->next = traverse->next->next;
                free(backup->key);
                free(backup->value);
                free(backup);
                me->size--;
                was_modified = true;
                continue;
            }
            traverse = traverse->next;
        }
        break;
    }
    return was_modified;
}

/**
 * Clears the key-value pairs from the unordered multi-map.
 *
 * @param me The unordered multi-map to clear.
 *
 * @return 0       No error.
 *         -ENOMEM Out of memory.
 */
int unordered_multimap_clear(unordered_multimap me)
{
    for (int i = 0; i < me->capacity; i++) {
        struct node *traverse = me->buckets[i];
        while (traverse != NULL) {
            struct node *const backup = traverse;
            traverse = traverse->next;
            free(backup->key);
            free(backup->value);
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
 * Frees the unordered multi-map memory.
 *
 * @param me The unordered multi-map to free from memory.
 *
 * @return NULL
 */
unordered_multimap unordered_multimap_destroy(unordered_multimap me)
{
    unordered_multimap_clear(me);
    free(me->iterate_key);
    free(me->buckets);
    free(me);
    return NULL;
}
