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

const int STARTING_BUCKETS = 8;
const double RESIZE_AT = 0.75;
const double RESIZE_RATIO = 1.5;

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
    init->buckets = calloc(STARTING_BUCKETS, sizeof(struct node));
    if (init->buckets == NULL) {
        free(init);
        return NULL;
    }
    return init;
}

void unordered_set_rehash(unordered_set me)
{
    // TODO: rehash
}

int unordered_set_size(unordered_set me)
{
    return me->size;
}

bool unordered_set_is_empty(unordered_set me)
{
    return unordered_set_size(me) == 0;
}

static bool unordered_set_is_equal(unordered_set me,
                                   const struct node *const one,
                                   const struct node *const two)
{
    return one->hash == two->hash && me->comparator(one->key, two->key) == 0;
}

static bool unordered_set_is_add_new(unordered_set me, struct node *const add)
{
    const int index = (int) (add->hash % me->capacity);
    if (me->buckets[index] == NULL) {
        me->buckets[index] = add;
    } else {
        struct node *traverse = me->buckets[index];
        if (unordered_set_is_equal(me, traverse, add)) {
            return true;
        }
        while (traverse->next != NULL) {
            traverse = traverse->next;
            if (unordered_set_is_equal(me, traverse, add)) {
                return true;
            }
        }
        traverse->next = add;
    }
    return false;
}

int unordered_set_add(unordered_set me, void *const key)
{
    struct node *const init = malloc(sizeof(struct node));
    if (init == NULL) {
        return -ENOMEM;
    }
    init->key = malloc(me->key_size);
    if (init->key == NULL) {
        free(init);
        return -ENOMEM;
    }
    memcpy(init->key, key, me->key_size);
    init->hash = me->hash(key);
    init->next = NULL;
    if (unordered_set_is_add_new(me, init)) {
        free(init->key);
        free(init);
        return 0;
    }
    me->size++;
    if (me->size >= RESIZE_AT * me->capacity) {
        // TODO: make this work
#if 0
        const int old_capacity = me->capacity;
        me->capacity *= RESIZE_RATIO;
        struct node **old_buckets = me->buckets;
        me->buckets = calloc((size_t) me->capacity, sizeof(struct node));
        if (me->buckets == NULL) {
            me->buckets = old_buckets;
            return -ENOMEM;
        }
        for (int i = 0; i < old_capacity; i++) {
            struct node *traverse = old_buckets[i];
            while (traverse != NULL) {
                unordered_set_is_add_new(me, traverse);
                traverse = traverse->next;
            }
        }
        free(old_buckets);
#endif
    }
    return 0;
}

bool unordered_set_contains(unordered_set me, void *const key)
{
    const int index = (int) (me->hash(key) % me->capacity);
    if (me->buckets[index] == NULL) {
        return false;
    }
    const struct node check = {.key = key, .hash = me->hash(key)};
    const struct node *traverse = me->buckets[index];
    if (unordered_set_is_equal(me, traverse, &check)) {
        return true;
    }
    while (traverse->next != NULL) {
        traverse = traverse->next;
        if (unordered_set_is_equal(me, traverse, &check)) {
            return true;
        }
    }
    return false;
}

bool unordered_set_remove(unordered_set me, void *key)
{
    const int index = (int) (me->hash(key) % me->capacity);
    if (me->buckets[index] == NULL) {
        return false;
    }
    const struct node check = {.key = key, .hash = me->hash(key)};
    struct node *traverse = me->buckets[index];
    if (unordered_set_is_equal(me, traverse, &check)) {
        me->buckets[index] = traverse->next;
        free(traverse->key);
        free(traverse);
        me->size--;
        return true;
    }
    while (traverse->next != NULL) {
        if (unordered_set_is_equal(me, traverse->next, &check)) {
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

void unordered_set_clear(unordered_set me)
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
}

unordered_set unordered_set_destroy(unordered_set me)
{
    unordered_set_clear(me);
    free(me->buckets);
    free(me);
    return NULL;
}
