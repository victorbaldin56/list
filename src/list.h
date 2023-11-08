#ifndef LIST_LIST_H_
#define LIST_LIST_H_

#include <stddef.h>
#include <sys/types.h>

struct List {
    int *data;
    ssize_t *prev;
    ssize_t *next;
    ssize_t head;
    ssize_t tail;
    ssize_t free;
    ssize_t size;
};

const int LC_BAD_ALLOC = -1;

int ListCtor(struct List *list, size_t size);

void ListDtor(struct List *list);

enum ListErrors {
    LIST_SIZE_NEGATIVE = -9,
    LIST_TAIL_NEGATIVE = -8,
    LIST_HEAD_NEGATIVE = -7,
    LIST_FREE_NEGATIVE = -6,
    LIST_NEXT_NEGATIVE = -5,
    LIST_NOT_RINGED    = -4,
    LIST_DATA_NULL     = -3,
    LIST_PREV_NULL     = -2,
    LIST_NEXT_NULL     = -1,
    LIST_OK            =  0,
};

ListErrors ListVerify(const struct List *list);

const int LD_FILE_CREATE_FAILED = -2;

int ListDump(const struct List *list, const char *file, const char *func,
             size_t line);

#ifndef NDEBUG
#define LIST_DUMP(list)                                 \
    do {                                                \
        ListDump(list, __FILE__, __func__, __LINE__);   \
    } while (0);
#else
#define LIST_DUMP(list) ;
#endif

ssize_t ListInsertAtTail(struct List *list, int val);

ssize_t ListInsertAtHead(struct List *list, int val);

ssize_t ListInsertAfter(struct List *list, int val, ssize_t idx);

ssize_t ListInsertBefore(struct List *list, int val, ssize_t idx);

void ListDeleteFromHead(struct List *list);

void ListDeleteFromTail(struct List *list);

void ListDeleteBefore(struct List *list, ssize_t idx);

void ListDeleteAfter(struct List *list, ssize_t idx);

#endif
