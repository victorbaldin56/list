#include <stdio.h>

#include "list.h"

const size_t LIST_INITIAL_SIZE = 20;

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
    LIST_DUMP(&list);

    ListDtor(&list);
    return 0;
}
