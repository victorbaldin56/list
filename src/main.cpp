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
    ListInsertAfter(&list, 100, 18);
    ListInsertAfter(&list, -10, 10);
    LIST_DUMP(&list);

    ListDtor(&list);
    return 0;
}
