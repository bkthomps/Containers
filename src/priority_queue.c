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
#include "priority_queue.h"

static const int START_SPACE = 8;
static const double RESIZE_RATIO = 1.5;

struct _priority_queue {
    size_t data_size;
    int offset;
    int space;
    void *storage;
    int (*comparator)(const void *const one, const void *const two);
};

priority_queue priority_queue_init(const size_t data_size,
                                   int (*comparator)(const void *const,
                                                     const void *const))
{
    return NULL; // TODO
}

int priority_queue_size(priority_queue me)
{
    return 0; // TODO
}

bool priority_queue_is_empty(priority_queue me)
{
    return false; // TODO
}

int priority_queue_push(priority_queue me, void *const data)
{
    return 0; // TODO
}

bool priority_queue_pop(void *const data, priority_queue me)
{
    return false; // TODO
}

bool priority_queue_front(void *const data, priority_queue me)
{
    return false; // TODO
}

void priority_queue_clear(priority_queue me)
{
    // TODO
}

priority_queue priority_queue_destroy(priority_queue me)
{
    return NULL; // TODO
}
