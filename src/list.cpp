#include "list.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dump.h"
#include "recalloc.h"

static const size_t REALLOC_COEFF = 2;
static const int LR_BAD_REALLOC = -1;

static int ListRealloc(struct List *list, size_t newsize);

static inline size_t PutValToList(struct List *list, int val);

int ListCtor(struct List *list, size_t size)
{
    assert(list);

    list->data = (int *)calloc(size, sizeof(*list->data));
    list->prev = (size_t *)calloc(size, sizeof(*list->prev));
    list->next = (size_t *)calloc(size, sizeof(*list->next));
    if (!list->data || !list->prev || !list->next)
        return LC_BAD_ALLOC;

    list->size = size;
    list->head = list->tail = 0;
    list->free = 1;
    for (size_t i = 1; i < size - 1; i++) {
        list->next[i] = i + 1;
    }

    return 0;
}

void ListDtor(struct List *list)
{
    assert(list);

    free(list->data);
    free(list->prev);
    free(list->next);

    list->data = NULL;
    list->prev = NULL;
    list->next = NULL;
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

    if (list->next[0] != 0)
        return LIST_NOT_RINGED;

    return LIST_OK;
}

int ListDump(const struct List *list, const char *file, const char *func,
             size_t line)
{
    assert(list);
    assert(file);
    assert(func);

    FILE *fp = HTCreateLog();
    if (!fp) {
        perror("ListDump");
        return LD_FILE_CREATE_FAILED;
    }

    fprintf(fp, "<!DOCTYPE html>\n");

    fprintf(fp, "<pre>\nList dump from function %s, file %s, line %zu\n",
            func, file, line);

    fprintf(fp, "size: %zu\n", list->size);
    fprintf(fp, "head: %zu\n", list->head);
    fprintf(fp, "tail: %zu\n", list->tail);
    fprintf(fp, "free: %zu\n", list->free);
    fprintf(fp, "</pre>\n");

    fprintf(fp, "data:\n");
    HTDumpArray(fp, list->data, list->size);

    fprintf(fp, "prev:\n");
    HTDumpArray(fp, (int *)list->prev, list->size);

    fprintf(fp, "next:\n");
    HTDumpArray(fp, (int *)list->next, list->size);

    fclose(fp);
    return 0;
}

size_t ListInsertAtTail(struct List *list, int val)
{
    size_t curidx = ListInsertAfter(list, val, list->tail);
    list->tail = curidx;
    return curidx;
}

size_t ListInsertAtHead(struct List *list, int val)
{
    size_t curidx = ListInsertBefore(list, val, list->head);
    list->head = curidx;
    return curidx;
}

size_t ListInsertAfter(struct List *list, int val, size_t idx)
{
    if (ListVerify(list) != LIST_OK)
        return 0;

    size_t curidx = PutValToList(list, val);
    list->next[idx] = curidx;
    list->prev[curidx] = idx;
    return curidx;
}

size_t ListInsertBefore(struct List *list, int val, size_t idx)
{
    if (ListVerify(list) != LIST_OK)
        return 0;

    size_t curidx = PutValToList(list, val);
    list->prev[idx] = curidx;
    list->next[curidx] = idx;
    return curidx;
}

static int ListRealloc(struct List *list, size_t newsize)
{
    int    *newdata = (int *)   __recalloc(list->data, newsize,
                                           sizeof(*list->data));
    size_t *newprev = (size_t *)__recalloc(list->prev, newsize,
                                           sizeof(*list->prev));
    size_t *newnext = (size_t *)__recalloc(list->next, newsize,
                                           sizeof(*list->next));
    if (!newdata || !newprev || !newprev)
        return LR_BAD_REALLOC;

    list->data = newdata;
    list->prev = newprev;
    list->next = newnext;

    list->size = newsize;
    return 0;
}

static inline size_t PutValToList(struct List *list, int val)
{
    if (!list->free) {
        if (ListRealloc(list, list->size * REALLOC_COEFF) != 0)
            return 0;
    }
    size_t curidx = list->free;
    list->free = list->next[curidx];
    list->data[curidx] = val;
    return curidx;
}
