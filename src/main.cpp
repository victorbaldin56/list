#include <stdio.h>

#include "debug.h"
#include "list.h"

int main()
{
    struct List list = {};
    if (ListCtor(&list) != 0) {
        fprintf(stderr, "list calloc failed\n");
        return LC_BAD_ALLOC;
    }

    LIST_DUMP(&list, "newly created list");

    ListInsertAtHead(&list, 90);
    LIST_DUMP(&list, "first head insertion");
    ListInsertAtTail(&list, 80);
    LIST_DUMP(&list, "first tail insertion");
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
    LIST_DUMP(&list, "...");
    ssize_t i = ListFind(&list, 2);
    printf("The second list element: (%zd, %d)\n",
           i, list.data[i]);

    printf("-10 is here: %zd\n", __ListSlowSlowLinearSearch__(&list, -10));
    for (size_t j = 0; j < 100; j++) {
        ListInsertAtHead(&list, 1000);
    }
    LIST_DUMP(&list, "insertted 100 elements to head in the loop");
    for (size_t j = 0; j < 10; j++) {
        ListInsertAtTail(&list, -1000);
    }
    LIST_DUMP(&list, "inserted 10 elements to the tail in the loop");
    ListDtor(&list);
    return 0;
}
