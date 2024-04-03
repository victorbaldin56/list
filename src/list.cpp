#include "../include/list.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const size_t REALLOC_COEFF = 2;
static const int LR_BAD_REALLOC   = -1;

static int ListRealloc(struct List *list, size_t newsize);

static inline ssize_t ListAddValue(struct List *list, const void* val);

static inline void ListFreeNode(struct List *list, ssize_t idx);

int ListCtor(struct List *list, size_t elem_size)
{
    assert(list);
    list->data = calloc(1, elem_size);
    list->prev = (ssize_t *)calloc(1, sizeof(*list->prev));
    list->next = (ssize_t *)calloc(1, sizeof(*list->next));
    if (!list->data || !list->prev || !list->next) {
        free(list->data);
        free(list->prev);
        free(list->next);
        return LC_BAD_ALLOC;
    }
    list->elem_size = elem_size;
    list->capacity = 1;
    list->next[0] = list->prev[0] = 0;
    list->free = 0;
    list->size = 0;
    return 0;
}


void ListDtor(struct List *list)
{
    LIST_ASSERT(list);
    free(list->data);
    free(list->prev);
    free(list->next);
}

ListErrors ListVerify(const struct List *list)
{
    assert(list);
    if (!list->data)
        return LIST_DATA_NULL;
    if (!list->prev)
        return LIST_PREV_NULL;
    if (!list->next)
        return LIST_NEXT_NULL;
    for (ssize_t i = 0; i < list->capacity; i++) {
        if (list->next[i] < 0)
            return LIST_NEXT_NEGATIVE;
    }
    if (list->free < 0)
        return LIST_FREE_NEGATIVE;
    if (list->capacity < 0)
        return LIST_SIZE_NEGATIVE;

    return LIST_OK;
}

ssize_t ListInsertAtTail(struct List *list, const void* val)
{
    return ListInsertAfter(list, val, list->prev[0]);
}

ssize_t ListInsertAtHead(struct List *list, const void* val)
{
    return ListInsertBefore(list, val, list->next[0]);
}

ssize_t ListInsertAfter(struct List *list, const void* val, ssize_t idx)
{
    LIST_ASSERT(list);
    ssize_t curidx = ListAddValue(list, val);
    list->next[curidx] = list->next[idx];
    list->prev[list->next[idx]] = curidx;
    list->next[idx] = curidx;
    list->prev[curidx] = idx;
    return curidx;
}

ssize_t ListInsertBefore(struct List *list, const void* val, ssize_t idx)
{
    LIST_ASSERT(list);
    ssize_t curidx = ListAddValue(list, val);
    list->next[list->prev[idx]] = curidx;
    list->prev[curidx] = list->prev[idx];
    list->prev[idx] = curidx;
    list->next[curidx] = idx;
    return curidx;
}

void ListDeleteFromHead(struct List *list)
{
    ListDeleteBefore(list, list->next[list->next[0]]);
}

void ListDeleteFromTail(struct List *list)
{
    ListDeleteAfter(list, list->prev[list->prev[0]]);
}

void ListDeleteBefore(struct List *list, ssize_t idx)
{
    LIST_ASSERT(list);
    ssize_t delidx = list->prev[list->prev[idx]];
    ListFreeNode(list, list->prev[idx]);
    list->prev[idx] = delidx;
    list->next[delidx] = idx;
}

void ListDeleteAfter(struct List *list, ssize_t idx)
{
    LIST_ASSERT(list);
    ssize_t delidx = list->next[list->next[idx]];
    ListFreeNode(list, list->next[idx]);
    list->next[idx] = delidx;
    list->prev[delidx] = idx;
}

static int ListRealloc(struct List *list, size_t newsize)
{
    LIST_ASSERT(list);
    void *newdata     = realloc(list->data, newsize * list->elem_size);
    ssize_t *newprev = (ssize_t *)realloc(list->prev,
                                          newsize * sizeof(*list->prev));
    ssize_t *newnext = (ssize_t *)realloc(list->next,
                                          newsize * sizeof(*list->next));
    if (!newdata || !newprev || !newprev) {
        free(newnext);
        free(newprev);
        free(newdata);
        return LR_BAD_REALLOC;
    }
    list->data = newdata;
    list->prev = newprev;
    list->next = newnext;
    list->free = list->capacity;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
    for (ssize_t i = list->capacity; i < newsize - 1; i++) {
        list->next[i] = i + 1;
        list->prev[i] = -1;
    }
#pragma GCC diagnostic pop
    list->next[newsize - 1] = 0;
    list->prev[newsize - 1] = -1;

    list->capacity = (ssize_t)newsize;
    return 0;
}

static inline ssize_t ListAddValue(struct List *list, const void* val)
{
    LIST_ASSERT(list);
    if (!list->free) {
        if (ListRealloc(list, (size_t)list->capacity * REALLOC_COEFF) != 0)
            return 0;
    }
    ssize_t curidx = list->free;
    list->free = list->next[curidx];
    ++list->size;
    memcpy(ListGetIterator(list, curidx), val, list->elem_size);
    return curidx;
}

static inline void ListFreeNode(struct List *list, ssize_t idx)
{
    LIST_ASSERT(list);
    list->prev[idx] = -1;
    --list->size;
    list->next[idx] = list->free;
    list->free = idx;
}
