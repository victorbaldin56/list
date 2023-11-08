#include "list.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const size_t REALLOC_COEFF = 2;
static const int LR_BAD_REALLOC =  -1;

static int ListRealloc(struct List *list, size_t newsize);

static inline ssize_t ListAddValue(struct List *list, int val);

static inline void ListFreeNode(struct List *list, ssize_t idx);

int ListCtor(struct List *list, size_t size)
{
    assert(list);

    list->data = (int *)    calloc(size, sizeof(*list->data));
    list->prev = (ssize_t *)calloc(size, sizeof(*list->prev));
    list->next = (ssize_t *)calloc(size, sizeof(*list->next));
    if (!list->data || !list->prev || !list->next)
        return LC_BAD_ALLOC;

    list->size = (ssize_t)size;
    list->head = list->tail = 1;
    list->free = 1;
    for (ssize_t i = 1; i < list->size - 1; i++) {
        list->prev[i] = -1;
        list->next[i] = i + 1;
    }
    list->prev[size - 1] = -1;

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
    for (ssize_t i = 0; i < list->size; i++) {
        if (list->next[i] < 0)
            return LIST_NEXT_NEGATIVE;
    }
    if (list->free < 0)
        return LIST_FREE_NEGATIVE;
    if (list->head < 0)
        return LIST_HEAD_NEGATIVE;
    if (list->tail < 0)
        return LIST_TAIL_NEGATIVE;
    if (list->size < 0)
        return LIST_SIZE_NEGATIVE;

    return LIST_OK;
}

int ListDump(const struct List *list, const char *file, const char *func,
             size_t line)
{
    assert(list);
    assert(file);
    assert(func);

    FILE *fp = fopen("dump.dot", "w");
    if (!fp) {
        perror("ListDump");
        return LD_FILE_CREATE_FAILED;
    }
    fprintf(fp,
            "digraph List {\n\trankdir = LR;\n\tnode [shape = Mrecord];\n");
    fprintf(fp, "0 ");
    for (ssize_t i = 0; i < list->size; i++) {
        fprintf(fp, "-> %zd", i);
    }
    fprintf(fp, "[arrowsize = 0.0, weight = 100000, color = \"#FFFFFF\"];\n");
    for (ssize_t i = 0; i < list->size; i++) {
        fprintf(fp, "\t%zd [shape = Mrecord, "
                "style = filled, ", i);
        if (list->prev[i] == -1)
            fprintf(fp, "fillcolor = cyan, ");
        else if (i != 0)
            fprintf(fp, "fillcolor = orange, ");
        else
            fprintf(fp, "fillcolor = red, ");
        fprintf(fp,  "label = \"idx: %zd | data: %d | next: %zd | "
                "prev: %zd\"];\n",
                i, list->data[i], list->next[i], list->prev[i]);
    }
    for (ssize_t i = 0; i < list->size; i++) {
        fprintf(fp, "\t%zd -> %zd;\n", i, list->next[i]);
    }
    for (ssize_t i = 0; i < list->size; i++) {
        if (list->prev[i] != -1)
            fprintf(fp, "\t%zd -> %zd;\n", i, list->prev[i]);
    }
    fprintf(fp, "\tlabel = \"ListDump from function %s, %s:%zu\";\n"
            "All[shape = Mrecord, label = \"size = %zd | head = %zd |"
            " tail = %zd | free = %zd\"];}\n",
            func, file, line,
            list->size, list->head, list->tail, list->free);
    fclose(fp);
    system("dot -T png dump.dot -o dump.png");

    return 0;
}

ssize_t ListInsertAtTail(struct List *list, int val)
{
    return ListInsertAfter(list, val, list->tail);
}

ssize_t ListInsertAtHead(struct List *list, int val)
{
    return ListInsertBefore(list, val, list->head);
}

ssize_t ListInsertAfter(struct List *list, int val, ssize_t idx)
{
    if (ListVerify(list) != LIST_OK)
        return 0;

    ssize_t curidx = ListAddValue(list, val);
    if (idx == list->tail) {
        list->tail = curidx;
        list->next[list->tail] = 0;
    }
    list->next[curidx] = list->next[idx];
    list->prev[list->next[idx]] = curidx;
    list->next[idx] = curidx;
    list->prev[curidx] = idx;
    return curidx;
}

ssize_t ListInsertBefore(struct List *list, int val, ssize_t idx)
{
    if (ListVerify(list) != LIST_OK)
        return 0;

    ssize_t curidx = ListAddValue(list, val);
    if (idx == list->head) {
        list->head = curidx;
        list->prev[list->head] = 0;
    } else {
        list->next[list->prev[idx]] = curidx;
    }
    list->prev[curidx] = list->prev[idx];
    list->prev[idx] = curidx;
    list->next[curidx] = idx;
    return curidx;
}

void ListDeleteFromHead(struct List *list)
{
    ListDeleteBefore(list, list->next[list->head]);
}

void ListDeleteFromTail(struct List *list)
{
    ListDeleteAfter(list, list->prev[list->tail]);
}

void ListDeleteBefore(struct List *list, ssize_t idx)
{
    assert(list);

    if (list->prev[idx] == list->head) {
        list->prev[list->head] = -1;
        ssize_t headidx = list->head;
        list->head = list->next[list->head];
        list->next[headidx] = list->free;
        list->prev[list->head] = 0;
        list->free = headidx;
    } else {
        ssize_t delidx = list->prev[list->prev[idx]];
        ListFreeNode(list, list->prev[idx]);
        list->prev[idx] = delidx;
        list->next[delidx] = idx;
    }
}

void ListDeleteAfter(struct List *list, ssize_t idx)
{
    assert(list);

    if (list->next[idx] == list->tail) {
        ssize_t tailidx = list->tail;
        list->tail = list->prev[list->tail];
        list->next[tailidx] = list->free;
        list->prev[tailidx] = -1;
        list->next[list->tail] = 0;
        list->free = tailidx;
    } else {
        ssize_t delidx = list->next[list->next[idx]];
        ListFreeNode(list, list->next[idx]);
        list->next[idx] = delidx;
        list->prev[delidx] = idx;
    }
}

ssize_t ListFind(const struct List *list, size_t elnum)
{
    assert(list);

    ssize_t j = list->head;
    for (size_t i = 0; i < elnum - 1; i++) {
        j = list->next[j];
    }
    return j;
}

ssize_t __ListSlowSlowLinearSearch__(const struct List *list, int val)
{
    ssize_t j = list->head;
    for ( ; list->data[j] != val; ) {
        j = list->next[j];
    }
    return j;
}

static int ListRealloc(struct List *list, size_t newsize)
{
    int     *newdata = (int *)   realloc(list->data,
                                         newsize * sizeof(*list->data));
    ssize_t *newprev = (ssize_t *)realloc(list->prev,
                                          newsize * sizeof(*list->prev));
    ssize_t *newnext = (ssize_t *)realloc(list->next,
                                          newsize * sizeof(*list->next));
    if (!newdata || !newprev || !newprev)
        return LR_BAD_REALLOC;

    list->data = newdata;
    list->prev = newprev;
    list->next = newnext;
    list->free = list->size;
    for (ssize_t i = list->size; i < newsize - 1; i++) {
        list->next[i] = i + 1;
        list->prev[i] = -1;
    }
    list->next[newsize - 1] = 0;
    list->prev[newsize - 1] = -1;

    list->size = (ssize_t)newsize;
    return 0;
}

static inline ssize_t ListAddValue(struct List *list, int val)
{
    if (!list->free) {
        if (ListRealloc(list, (size_t)list->size * REALLOC_COEFF) != 0)
            return 0;
    }
    ssize_t curidx = list->free;
    list->free = list->next[curidx];
    list->data[curidx] = val;
    return curidx;
}

static inline void ListFreeNode(struct List *list, ssize_t idx)
{
    assert(list);

    list->prev[idx] = -1;
    list->next[idx] = list->free;
    list->free = idx;
}
