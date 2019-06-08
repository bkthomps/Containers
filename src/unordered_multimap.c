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
#include "include/unordered_multimap.h"

static const int STARTING_BUCKETS = 8;
static const double RESIZE_AT = 0.75;
static const double RESIZE_RATIO = 1.5;

struct internal_unordered_multimap {
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

/*
 * Gets the hash by first calling the user-defined hash, and then using a
 * second hash to prevent hashing clusters if the user-defined hash is
 * sub-optimal.
 */
static unsigned long unordered_multimap_hash(unordered_multimap me,
                                             const void *const key)
{
    unsigned long hash = me->hash(key);
    hash ^= (hash >> 20UL) ^ (hash >> 12UL);
    return hash ^ (hash >> 7UL) ^ (hash >> 4UL);
}

/**
 * Initializes an unordered multi-map.
 *
 * @param key_size         the size of each key in the unordered multi-map; must
 *                         be positive
 * @param value_size       the size of each value in the unordered multi-map;
 *                         must be positive
 * @param hash             the hash function which computes the hash from key;
 *                         must not be NULL
 * @param key_comparator   the comparator function which compares two keys; must
 *                         not be NULL
 * @param value_comparator the comparator function which compares two values;
 *                         must not be NULL
 *
 * @return the newly-initialized unordered multi-map, or NULL if it was not
 *         successfully initialized due to either invalid input arguments or
 *         memory allocation error
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
    struct internal_unordered_multimap *init;
    if (key_size == 0 || value_size == 0
        || !hash || !key_comparator || !value_comparator) {
        return NULL;
    }
    init = malloc(sizeof(struct internal_unordered_multimap));
    if (!init) {
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
    if (!init->buckets) {
        free(init);
        return NULL;
    }
    init->iterate_hash = 0;
    init->iterate_key = calloc(1, init->key_size);
    if (!init->iterate_key) {
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
    struct node *traverse;
    const int index = (int) (add->hash % me->capacity);
    add->next = NULL;
    if (!me->buckets[index]) {
        me->buckets[index] = add;
        return;
    }
    traverse = me->buckets[index];
    while (traverse->next) {
        traverse = traverse->next;
    }
    traverse->next = add;
}

/**
 * Rehashes all the keys in the unordered multi-map. Used when storing
 * references and changing the keys. This should rarely be used.
 *
 * @param me the unordered multi-map to rehash
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_multimap_rehash(unordered_multimap me)
{
    int i;
    struct node **old_buckets = me->buckets;
    me->buckets = calloc((size_t) me->capacity, sizeof(struct node *));
    if (!me->buckets) {
        me->buckets = old_buckets;
        return -ENOMEM;
    }
    for (i = 0; i < me->capacity; i++) {
        struct node *traverse = old_buckets[i];
        while (traverse) {
            struct node *const backup = traverse->next;
            traverse->hash = unordered_multimap_hash(me, traverse->key);
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
 * @param me the unordered multi-map to check
 *
 * @return the size of the unordered multi-map
 */
int unordered_multimap_size(unordered_multimap me)
{
    return me->size;
}

/**
 * Determines whether or not the unordered multi-map is empty.
 *
 * @param me the unordered multi-map to check
 *
 * @return 1 if the unordered multi-map is empty, otherwise 0
 */
int unordered_multimap_is_empty(unordered_multimap me)
{
    return unordered_multimap_size(me) == 0;
}

/*
 * Increases the size of the multi-map and redistributes the nodes.
 */
static int unordered_multimap_resize(unordered_multimap me)
{
    int i;
    const int old_capacity = me->capacity;
    const int new_capacity = (int) (me->capacity * RESIZE_RATIO);
    struct node **old_buckets = me->buckets;
    me->buckets = calloc((size_t) new_capacity, sizeof(struct node *));
    if (!me->buckets) {
        me->buckets = old_buckets;
        return -ENOMEM;
    }
    me->capacity = new_capacity;
    for (i = 0; i < old_capacity; i++) {
        struct node *traverse = old_buckets[i];
        while (traverse) {
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
static int unordered_multimap_is_equal(unordered_multimap me,
                                       const struct node *const item,
                                       const unsigned long hash,
                                       const void *const key)
{
    return item->hash == hash && me->key_comparator(item->key, key) == 0;
}

/*
 * Creates an element to add.
 */
static struct node *unordered_multimap_create_element(unordered_multimap me,
                                                      const unsigned long hash,
                                                      const void *const key,
                                                      const void *const value)
{
    struct node *const init = malloc(sizeof(struct node));
    if (!init) {
        return NULL;
    }
    init->key = malloc(me->key_size);
    if (!init->key) {
        free(init);
        return NULL;
    }
    memcpy(init->key, key, me->key_size);
    init->value = malloc(me->value_size);
    if (!init->value) {
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
 * Adds a key-value pair to the unordered multi-map. The pointer to the key and
 * value being passed in should point to the key and value type which this
 * unordered multi-map holds. For example, if this unordered multi-map holds
 * integer keys and values, the key and value pointer should be a pointer to an
 * integer. Since the key and value are being copied, the pointer only has to be
 * valid when this function is called.
 *
 * @param me    the unordered multi-map to add to
 * @param key   the key to add
 * @param value the value to add
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_multimap_put(unordered_multimap me,
                           void *const key,
                           void *const value)
{
    const unsigned long hash = unordered_multimap_hash(me, key);
    int index;
    if (me->size + 1 >= RESIZE_AT * me->capacity) {
        const int rc = unordered_multimap_resize(me);
        if (rc != 0) {
            return rc;
        }
    }
    index = (int) (hash % me->capacity);
    if (!me->buckets[index]) {
        me->buckets[index] =
                unordered_multimap_create_element(me, hash, key, value);
        if (!me->buckets[index]) {
            return -ENOMEM;
        }
    } else {
        struct node *traverse = me->buckets[index];
        while (traverse->next) {
            traverse = traverse->next;
        }
        traverse->next =
                unordered_multimap_create_element(me, hash, key, value);
        if (!traverse->next) {
            return -ENOMEM;
        }
    }
    me->size++;
    return 0;
}

/**
 * Creates the iterator for the specified key. To iterate over the values, keep
 * getting the next value. Between starting and iterations, the unordered
 * multi-map must not be mutated. The pointer to the key being passed in should
 * point to the key type which this unordered multi-map holds. For example, if
 * this unordered multi-map holds key integers, the key pointer should be a
 * pointer to an integer. Since the key is being copied, the pointer only has
 * to be valid when this function is called.
 *
 * @param me  the unordered multi-map to start the iterator for
 * @param key the key to start the iterator for
 */
void unordered_multimap_get_start(unordered_multimap me, void *const key)
{
    int index;
    struct node *traverse;
    me->iterate_hash = unordered_multimap_hash(me, key);
    memcpy(me->iterate_key, key, me->key_size);
    me->iterate_element = NULL;
    index = (int) (me->iterate_hash % me->capacity);
    traverse = me->buckets[index];
    while (traverse) {
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
 * iterations. The pointer to the value being obtained should point to the value
 * type which this unordered multi-map holds. For example, if this unordered
 * multi-map holds value integers, the value pointer should be a pointer to an
 * integer. Since the value is being copied, the pointer only has to be valid
 * when this function is called.
 *
 * @param value the value to be copied to from iteration
 * @param me    the unordered multi-map to iterate over
 *
 * @return 1 if there exist no more values for the key which is being iterated
 *         over, otherwise 0
 */
int unordered_multimap_get_next(void *const value, unordered_multimap me)
{
    struct node *item;
    struct node *traverse;
    if (!me->iterate_element) {
        return 0;
    }
    item = me->iterate_element;
    traverse = item->next;
    while (traverse) {
        if (unordered_multimap_is_equal(me, traverse, me->iterate_hash,
                                        me->iterate_key)) {
            me->iterate_element = traverse;
            memcpy(value, item->value, me->value_size);
            return 1;
        }
        traverse = traverse->next;
    }
    me->iterate_element = NULL;
    memcpy(value, item->value, me->value_size);
    return 1;
}

/**
 * Determines the number of times the key appears in the unordered multi-map.
 * The pointer to the key being passed in should point to the key type which
 * this unordered multi-map holds. For example, if this unordered multi-map
 * holds key integers, the key pointer should be a pointer to an integer. Since
 * the key is being copied, the pointer only has to be valid when this function
 * is called.
 *
 * @param me  the unordered multi-map to check for the key
 * @param key the key to check
 *
 * @return the number of times the key appears in the unordered multi-map
 */
int unordered_multimap_count(unordered_multimap me, void *const key)
{
    int count = 0;
    const unsigned long hash = unordered_multimap_hash(me, key);
    const int index = (int) (hash % me->capacity);
    const struct node *traverse = me->buckets[index];
    while (traverse) {
        if (unordered_multimap_is_equal(me, traverse, hash, key)) {
            count++;
        }
        traverse = traverse->next;
    }
    return count;
}

/**
 * Determines if the unordered multi-map contains the specified key. The pointer
 * to the key being passed in should point to the key type which this unordered
 * multi-map holds. For example, if this unordered multi-map holds key integers,
 * the key pointer should be a pointer to an integer. Since the key is being
 * copied, the pointer only has to be valid when this function is called.
 *
 * @param me  the unordered multi-map to check for the key
 * @param key the key to check
 *
 * @return 1 if the unordered multi-map contained the key, otherwise 0
 */
int unordered_multimap_contains(unordered_multimap me, void *const key)
{
    const unsigned long hash = unordered_multimap_hash(me, key);
    const int index = (int) (hash % me->capacity);
    const struct node *traverse = me->buckets[index];
    while (traverse) {
        if (unordered_multimap_is_equal(me, traverse, hash, key)) {
            return 1;
        }
        traverse = traverse->next;
    }
    return 0;
}

/**
 * Removes the key-value pair from the unordered multi-map if it contains it.
 * The pointer to the key and value being passed in should point to the key and
 * value type which this unordered multi-map holds. For example, if this
 * unordered multi-map holds integer keys and values, the key and value pointer
 * should be a pointer to an integer. Since the key and value are being copied,
 * the pointer only has to be valid when this function is called.
 *
 * @param me    the unordered multi-map to remove a key from
 * @param key   the key to remove
 * @param value the value to remove
 *
 * @return 1 if the unordered multi-map contained the key, otherwise 0
 */
int unordered_multimap_remove(unordered_multimap me,
                              void *const key,
                              void *const value)
{
    struct node *traverse;
    int is_key_equal;
    const unsigned long hash = unordered_multimap_hash(me, key);
    const int index = (int) (hash % me->capacity);
    if (!me->buckets[index]) {
        return 0;
    }
    traverse = me->buckets[index];
    is_key_equal = unordered_multimap_is_equal(me, traverse, hash, key);
    if (is_key_equal && me->value_comparator(traverse->value, value) == 0) {
        me->buckets[index] = traverse->next;
        free(traverse->key);
        free(traverse->value);
        free(traverse);
        me->size--;
        return 1;
    }
    while (traverse->next) {
        is_key_equal =
                unordered_multimap_is_equal(me, traverse->next, hash, key);
        if (is_key_equal && me->value_comparator(traverse->value, value) == 0) {
            struct node *const backup = traverse->next;
            traverse->next = traverse->next->next;
            free(backup->key);
            free(backup->value);
            free(backup);
            me->size--;
            return 1;
        }
        traverse = traverse->next;
    }
    return 0;
}

/**
 * Removes all the key-value pairs from the unordered multi-map specified by
 * the key. The pointer to the key being passed in should point to the key
 * type which this unordered multi-map holds. For example, if this unordered
 * multi-map holds key integers, the key pointer should be a pointer to an
 * integer. Since the key is being copied, the pointer only has to be valid
 * when this function is called.
 *
 * @param me  the unordered multi-map to remove a key-value pair from
 * @param key the key to remove
 *
 * @return 1 if the unordered multi-map contained the key, otherwise 0
 */
int unordered_multimap_remove_all(unordered_multimap me, void *const key)
{
    const unsigned long hash = unordered_multimap_hash(me, key);
    const int index = (int) (hash % me->capacity);
    int was_modified = 0;
    for (;;) {
        struct node *traverse = me->buckets[index];
        if (!traverse) {
            break;
        }
        if (unordered_multimap_is_equal(me, traverse, hash, key)) {
            me->buckets[index] = traverse->next;
            free(traverse->key);
            free(traverse->value);
            free(traverse);
            me->size--;
            was_modified = 1;
            continue;
        }
        while (traverse->next) {
            if (unordered_multimap_is_equal(me, traverse->next, hash, key)) {
                struct node *const backup = traverse->next;
                traverse->next = traverse->next->next;
                free(backup->key);
                free(backup->value);
                free(backup);
                me->size--;
                was_modified = 1;
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
 * @param me the unordered multi-map to clear
 *
 * @return 0       if no error
 * @return -ENOMEM if out of memory
 */
int unordered_multimap_clear(unordered_multimap me)
{
    int i;
    struct node **temp =
            calloc((size_t) STARTING_BUCKETS, sizeof(struct node *));
    if (!temp) {
        return -ENOMEM;
    }
    for (i = 0; i < me->capacity; i++) {
        struct node *traverse = me->buckets[i];
        while (traverse) {
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
    free(me->buckets);
    me->buckets = temp;
    return 0;
}

/**
 * Frees the unordered multi-map memory. Performing further operations after
 * calling this function results in undefined behavior.
 *
 * @param me the unordered multi-map to free from memory
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
