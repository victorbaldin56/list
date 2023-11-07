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
    fprintf(fp, "[arrowsize = 0.0, weight = 10000, color = \"#FFFFFF\"];\n");
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

int *ListInsertAtTail(struct List *list, int val)
{
    ssize_t tailidx = ListAddValue(list, val);
    list->next[tailidx] = 0;
    list->prev[tailidx] = list->tail;
    list->next[list->tail] = tailidx;
    list->tail = tailidx;
    return list->data + list->tail;
}

int *ListInsertAtHead(struct List *list, int val)
{
    ssize_t headidx = ListAddValue(list, val);
    list->prev[headidx] = 0;
    list->next[headidx] = list->head;
    list->prev[list->head] = headidx;
    list->head = headidx;
    return list->data + list->head;
}

int *ListInsertAfter(struct List *list, int val, ssize_t idx)
{
    if (ListVerify(list) != LIST_OK)
        return 0;
    if (list->prev[idx] == -1)
        return 0;

    ssize_t curidx = ListAddValue(list, val);
    list->next[idx] = curidx;
    list->prev[curidx] = idx;
    return list->data + curidx;
}

int *ListInsertBefore(struct List *list, int val, ssize_t idx)
{
    if (ListVerify(list) != LIST_OK)
        return 0;
    if (list->prev[idx] == -1)
        return 0;

    ssize_t curidx = ListAddValue(list, val);
    list->prev[idx] = curidx;
    list->next[curidx] = idx;
    return list->data + curidx;
}

void ListDeleteFromHead(struct List *list)
{
    assert(list);

    list->prev[list->head] = -1;
    ssize_t headidx = list->head;
    list->head = list->next[list->head];
    list->next[headidx] = list->free;
    list->prev[list->head] = 0;
    list->free = headidx;
}

void ListDeleteFromTail(struct List *list)
{
    assert(list);

    ssize_t tailidx = list->tail;
    list->tail = list->prev[list->tail];
    list->next[tailidx] = list->free;
    list->prev[tailidx] = -1;
    list->next[list->tail] = 0;
    list->free = tailidx;
}

static int ListRealloc(struct List *list, size_t newsize)
{
    int     *newdata  = (int *)   realloc(list->data,
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
