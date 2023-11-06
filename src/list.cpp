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

static inline size_t ListAddValue(struct List *list, int val);

int ListCtor(struct List *list, size_t size)
{
    assert(list);

    list->data = (int *)    calloc(size, sizeof(*list->data));
    list->prev = (ssize_t *)calloc(size, sizeof(*list->prev));
    list->next = (size_t *) calloc(size, sizeof(*list->next));
    if (!list->data || !list->prev || !list->next)
        return LC_BAD_ALLOC;

    list->size = size;
    list->head = list->tail = 1;
    list->free = 1;
    for (size_t i = 1; i < size - 1; i++) {
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
            "digraph List {\n\trankdir = LR;\n\tnode [shape = record];\n");
    fprintf(fp, "0 ");
    for (size_t i = 0; i < list->size; i++) {
        fprintf(fp, "-> %zu", i);
    }
    fprintf(fp, "[arrowsize = 0.0, weight = 10000, color = \"#FFFFFF\"];\n");
    for (size_t i = 0; i < list->size; i++) {
        fprintf(fp, "\t%zu [shape = Mrecord, "
                "style = filled, ", i);
        if (list->prev[i] == -1)
            fprintf(fp, "fillcolor = cyan, ");
        else if (i != 0)
            fprintf(fp, "fillcolor = orange, ");
        else
            fprintf(fp, "fillcolor = red, ");
        fprintf(fp,  "label = \"idx: %zu | data: %d | next: %zu | "
                "prev: %zd\"];\n",
                i, list->data[i], list->next[i], list->prev[i]);
    }
    for (size_t i = 0; i < list->size; i++) {
        fprintf(fp, "\t%zu -> %zu;\n", i, list->next[i]);
    }
    fprintf(fp, "\tlabel = \"ListDump from function %s, %s:%zu\";\n"
            "All[shape = Mrecord, label = \"size = %zu | head = %zu |"
            " tail = %zu | free = %zu\"];}\n",
            func, file, line,
            list->size, list->head, list->tail, list->free);
    fclose(fp);
    system("dot -T png dump.dot -o dump.png");

    return 0;
}

size_t ListInsertAtTail(struct List *list, int val)
{
    list->tail = ListInsertAfter(list, val, list->tail);
    return list->tail;
}

size_t ListInsertAtHead(struct List *list, int val)
{
    list->head = ListInsertBefore(list, val, list->head);
    return list->head;
}

size_t ListInsertAfter(struct List *list, int val, size_t idx)
{
    if (ListVerify(list) != LIST_OK)
        return 0;

    size_t curidx = ListAddValue(list, val);
    list->next[idx] = curidx;
    list->prev[curidx] = idx;
    return curidx;
}

size_t ListInsertBefore(struct List *list, int val, size_t idx)
{
    if (ListVerify(list) != LIST_OK)
        return 0;

    size_t curidx = ListAddValue(list, val);
    list->prev[idx] = curidx;
    list->next[curidx] = idx;
    return curidx;
}

static int ListRealloc(struct List *list, size_t newsize)
{
    int    *newdata  = (int *)    realloc(list->data,
                                          newsize * sizeof(*list->data));
    ssize_t *newprev = (ssize_t *)realloc(list->prev,
                                          newsize * sizeof(*list->prev));
    size_t *newnext  = (size_t *) realloc(list->next,
                                          newsize * sizeof(*list->next));
    if (!newdata || !newprev || !newprev)
        return LR_BAD_REALLOC;

    list->data = newdata;
    list->prev = newprev;
    list->next = newnext;

    list->size = newsize;
    return 0;
}

static inline size_t ListAddValue(struct List *list, int val)
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
