#ifndef LIST_LIST_H_
#define LIST_LIST_H_

#include <stddef.h>
#include <sys/types.h>
#include <stdlib.h>

#include "debug.h"

/**
 * @brief doubly-linked list implementation based on 3 arrays
 * @details All new list structures should be created
 * with constructor function, like this:
 * @code struct List list;
 *  if (ListCtor(&list) != 0) // initial list size that you want to set
 *      fprintf(stderr, "list alloc failed\n"); // ERROR handling
 * @endcode
 * Checking constructor return value is ABSOLUTELY needed, because there is
 * NO garantee of memory allocation success and in some cases constructor MAY
 * fail and return a non-zero value.
 * Without fully correct list initialization, all functions will NOT work
 * correctly and may cause program crash.
 * After use, do not forget to call ListDtor
 * to free memory used by List members:
 * @code ListDtor(&list);
 * @endcode
 * Passing a pointer to unitialized List object will cause undefined behaiviour.
*/
struct List {
    void *data;      ///< data array
    ssize_t *prev;  ///< previous indexes array
    ssize_t *next;  ///< next indexes array
    ssize_t free;   ///< the first free element of the list
    ssize_t capacity;   ///< size of the list
    size_t elem_size;
    size_t size;
};

const int LC_BAD_ALLOC = -1;

#define NEW_LIST(list, type) ListCtor(list, sizeof(type))

int ListCtor(struct List *list, size_t elem_size);

void ListDtor(struct List *list);

enum ListErrors {
    LIST_SIZE_NEGATIVE = -8,
    LIST_TAIL_NEGATIVE = -7,
    LIST_HEAD_NEGATIVE = -6,
    LIST_FREE_NEGATIVE = -5,
    LIST_NEXT_NEGATIVE = -4,
    LIST_DATA_NULL     = -3,
    LIST_PREV_NULL     = -2,
    LIST_NEXT_NULL     = -1,
    LIST_OK            =  0,
};

ListErrors ListVerify(const struct List *list);

const int LD_FILE_CREATE_FAILED = -2;

/** @name ListInsert
 * @brief Insert a value to the list
 * @param list a pointer to list structure
 * @param val value to insert
 * @return real index in list.data where val has been inserted
*/
/// @{
//* Insert a value to the tail of the given list */
ssize_t ListInsertAtTail(struct List *list, const void* val);

//* Insert a value to the head of the given list */
ssize_t ListInsertAtHead(struct List *list, const void* val);

//* Insert a value after the element with given real index */
ssize_t ListInsertAfter(struct List *list, const void* val, ssize_t idx);

//* @brief Insert a value before the element with given real index */
ssize_t ListInsertBefore(struct List *list, const void* val, ssize_t idx);
/// @}

void ListDeleteFromHead(struct List *list);

void ListDeleteFromTail(struct List *list);

void ListDeleteBefore(struct List *list, ssize_t idx);

void ListDeleteAfter(struct List *list, ssize_t idx);

inline void* ListGetIterator(const List* list, ssize_t idx)
{
    if (idx < 0)
        return nullptr;
    return (char*)list->data + (size_t)idx * list->elem_size;
}

inline List* ListNewArray(size_t n, size_t elem_size)
{
    List* lists = (List*)calloc(n, sizeof(*lists));
    if (!lists)
        return nullptr;

    for (size_t i = 0; i < n; ++i) {
        if (ListCtor(lists + i, elem_size) != 0) {
            free(lists);
            return nullptr;
        }
    }

    return lists;
}

inline void ListDeleteArray(List* lists, size_t n)
{
    for (size_t i = 0; i < n; ++i)
        ListDtor(lists + i);
    free(lists);
}

#endif
