#include <stdio.h>

#include "list.h"

const size_t LIST_INITIAL_SIZE = 5;

int main()
{
    struct List list = {};
    if (ListCtor(&list, LIST_INITIAL_SIZE) != 0) {
        fprintf(stderr, "list calloc failed\n");
        return LC_BAD_ALLOC;
    }

    ListInsertAtHead(&list, 90);
    ListInsertAtTail(&list, 80);
    ListInsertAtHead(&list, -10);
    ListInsertAtHead(&list, -20);
    ListDeleteFromTail(&list);
    ListDeleteFromTail(&list);
    ListInsertAfter(&list, 100, 4);
    ListInsertAtHead(&list, -98);
    ListInsertBefore(&list, -1, 1);
    ListDeleteBefore(&list, 3);
    ListDeleteBefore(&list, 4);
    ListInsertAtTail(&list, 666);
    ListDeleteAfter(&list, 4);
    LIST_DUMP(&list);

    ListDtor(&list);
    return 0;
}
