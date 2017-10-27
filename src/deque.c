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
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <errno.h>
#include "deque.h"

const int BLOCK_SIZE = 8;

struct _deque {
    size_t data_size;
    int start_index;
    int end_index;
    int block_count;
    struct node *block;
};

struct node {
    void *data;
};

static void deque_DEBUG(deque me) {
    for (int i = 0; i < me->block_count; i++) {
        if (me->block[i].data == NULL) {
            printf("NULL\n");
            continue;
        }
        for (int j = 0; j < BLOCK_SIZE; j++) {
            int *a = me->block[i].data + j * me->data_size;
            printf("%d, ", *a);
        }
        printf("\n");
    }
    printf("\n\n");
}

deque deque_init(const size_t data_size) {
    struct _deque *const init = malloc(sizeof(struct _deque));
    if (init == NULL) {
        return NULL;
    }
    init->data_size = data_size;
    init->start_index = BLOCK_SIZE / 2;
    init->end_index = init->start_index + 1;
    init->block_count = 1;
    init->block = malloc(sizeof(struct node));
    if (init->block == NULL) {
        return NULL;
    }
    init->block->data = malloc(BLOCK_SIZE * data_size);
    if (init->block->data == NULL) {
        return NULL;
    }
    return init;
}

int deque_size(deque me) {
    return me->end_index - me->start_index - 1;
}

bool deque_is_empty(deque me) {
    return deque_size(me) == 0;
}

int deque_trim(deque me) {
    deque_DEBUG(me);
    return -ENOSYS; // TODO
}

void deque_to_array(void *const array, deque me) {
    void *const temp = malloc(me->data_size);
    if (temp == NULL) {
        return;
    }
    for (int i = 0; i < deque_size(me); i++) {
        deque_get_at(temp, me, i);
        memcpy(array + i * me->data_size, temp, me->data_size);
    }
    free(temp);
}

int deque_push_front(deque me, void *const data) {
    int block_index = me->start_index / BLOCK_SIZE;
    int inner_index = me->start_index % BLOCK_SIZE;
    if (inner_index == -1) {
        block_index = -1;
        inner_index = BLOCK_SIZE - 1;
    }
    if (inner_index == BLOCK_SIZE - 1) {
        if (block_index == -1) {
            const int old_block_count = me->block_count;
            me->block_count = (int) ceil(1.5 * me->block_count);
            const int added_blocks = me->block_count - old_block_count;
            void *temp = realloc(me->block,
                                 me->block_count * sizeof(struct node));
            if (temp == NULL) {
                return -ENOMEM;
            }
            me->block = temp;
            memmove(&me->block[added_blocks],
                    me->block,
                    old_block_count * sizeof(struct node));
            block_index = added_blocks - 1;
            me->start_index += added_blocks * BLOCK_SIZE;
            me->end_index += added_blocks * BLOCK_SIZE;
            for (int i = 0; i < added_blocks; i++) {
                me->block[i].data = NULL;
            }
        }
        if (me->block[block_index].data == NULL) {
            me->block[block_index].data = malloc(BLOCK_SIZE * me->data_size);
            if (me->block[block_index].data == NULL) {
                return -ENOMEM;
            }
        }
    }
    memcpy(me->block[block_index].data + inner_index * me->data_size,
           data,
           me->data_size);
    me->start_index--;
    deque_DEBUG(me);
    return 0;
}

int deque_push_back(deque me, void *const data) {
    const int block_index = me->end_index / BLOCK_SIZE;
    const int inner_index = me->end_index % BLOCK_SIZE;
    if (inner_index == 0) {
        if (block_index == me->block_count) {
            me->block_count = (int) ceil(1.5 * me->block_count);
            void *temp = realloc(me->block,
                                 me->block_count * sizeof(struct node));
            if (temp == NULL) {
                return -ENOMEM;
            }
            me->block = temp;
            for (int i = block_index; i < me->block_count; i++) {
                me->block[i].data = NULL;
            }
        }
        if (me->block[block_index].data == NULL) {
            me->block[block_index].data = malloc(BLOCK_SIZE * me->data_size);
            if (me->block[block_index].data == NULL) {
                return -ENOMEM;
            }
        }
    }
    memcpy(me->block[block_index].data + inner_index * me->data_size,
           data,
           me->data_size);
    me->end_index++;
    deque_DEBUG(me);
    return 0;
}

int deque_pop_front(void *const data, deque me) {
    if (deque_is_empty(me)) {
        return -EINVAL;
    }
    const int block_index = me->start_index / BLOCK_SIZE;
    const int inner_index = me->start_index % BLOCK_SIZE;
    memcpy(data,
           me->block[block_index].data + inner_index * me->data_size,
           me->data_size);
    me->start_index++;
    deque_DEBUG(me);
    return 0;
}

int deque_pop_back(void *const data, deque me) {
    if (deque_is_empty(me)) {
        return -EINVAL;
    }
    const int block_index = me->end_index / BLOCK_SIZE;
    const int inner_index = me->end_index % BLOCK_SIZE;
    memcpy(data,
           me->block[block_index].data + inner_index * me->data_size,
           me->data_size);
    me->end_index--;
    deque_DEBUG(me);
    return 0;
}

int deque_set_first(deque me, void *const data) {
    return deque_set_at(me, 0, data);
}

int deque_set_at(deque me, int index, void *const data) {
    if (index < 0 || index >= deque_size(me)) {
        return -EINVAL;
    }
    index += me->start_index + 1;
    const int block_index = index / BLOCK_SIZE;
    const int inner_index = index % BLOCK_SIZE;
    memcpy(me->block[block_index].data + inner_index * me->data_size,
           data,
           me->data_size);
    return 0;
}

int deque_set_last(deque me, void *const data) {
    return deque_set_at(me, deque_size(me) - 1, data);
}

int deque_get_first(void *const data, deque me) {
    return deque_get_at(data, me, 0);
}

int deque_get_at(void *const data, deque me, int index) {
    if (index < 0 || index >= deque_size(me)) {
        return -EINVAL;
    }
    index += me->start_index + 1;
    const int block_index = index / BLOCK_SIZE;
    const int inner_index = index % BLOCK_SIZE;
    memcpy(data,
           me->block[block_index].data + inner_index * me->data_size,
           me->data_size);
    return 0;
}

int deque_get_last(void *const data, deque me) {
    return deque_get_at(data, me, deque_size(me) - 1);
}

int deque_clear(deque me) {
    deque_DEBUG(me);
    return -ENOSYS; // TODO
}

deque deque_destroy(deque me) {
    return NULL; // TODO
}
