#ifndef LIST_LIST_H_
#define LIST_LIST_H_

#include <stddef.h>
#include <sys/types.h>

/**
 * @brief doubly-linked list implementation based on 3 arrays
 * @details All new list structures should be created
 * with constructor function, like this:
 * @code struct List list;
 *  if (ListCtor(&list, size) != 0) // initial list size that you want to set
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
    int *data;      ///< data array
    ssize_t *prev;  ///< previous indexes array
    ssize_t *next;  ///< next indexes array
    ssize_t head;   ///< head of the list
    ssize_t tail;   ///< tail of the list
    ssize_t free;   ///< the first free element of the list
    ssize_t size;   ///< size of the list
};

const int LC_BAD_ALLOC = -1;

int ListCtor(struct List *list, size_t size);

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

int ListDump(const struct List *list, const char *file, const char *func,
             size_t line);

#ifndef NDEBUG
#define LIST_DUMP(list) {                               \
    ListDump(list, __FILE__, __func__, __LINE__);       \
}
#else
#define LIST_DUMP(list) ;
#endif

/** @name ListInsert
 * @brief Insert a value to the list
 * @param list a pointer to list structure
 * @param val value to insert
 * @return real index in list.data where val has been inserted
*/
/// @{
//* Insert a value to the tail of the given list */
ssize_t ListInsertAtTail(struct List *list, int val);

//* Insert a value to the head of the given list */
ssize_t ListInsertAtHead(struct List *list, int val);

//* Insert a value after the element with given real index */
ssize_t ListInsertAfter(struct List *list, int val, ssize_t idx);

//* @brief Insert a value before the element with given real index */
ssize_t ListInsertBefore(struct List *list, int val, ssize_t idx);
/// @}

void ListDeleteFromHead(struct List *list);

void ListDeleteFromTail(struct List *list);

void ListDeleteBefore(struct List *list, ssize_t idx);

void ListDeleteAfter(struct List *list, ssize_t idx);

ssize_t ListFind(const struct List *list, size_t elnum);

ssize_t __ListSlowSlowLinearSearch__(const struct List *list, int val);

#endif
