#ifndef LIST_LIST_H_
#define LIST_LIST_H_

#include <stddef.h>
#include <sys/types.h>

struct List {
    int *data;
    ssize_t *prev;
    size_t *next;
    size_t head;
    size_t tail;
    size_t free;
    size_t size;
};

const int LC_BAD_ALLOC = -1;

int ListCtor(struct List *list, size_t size);

void ListDtor(struct List *list);

enum ListErrors {
    LIST_NOT_RINGED = -4,
    LIST_DATA_NULL  = -3,
    LIST_PREV_NULL  = -2,
    LIST_NEXT_NULL  = -1,
    LIST_OK         =  0,
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

size_t ListInsertAtTail(struct List *list, int val);

size_t ListInsertAtHead(struct List *list, int val);

size_t ListInsertAfter(struct List *list, int val, size_t idx);

size_t ListInsertBefore(struct List *list, int val, size_t idx);

#endif
