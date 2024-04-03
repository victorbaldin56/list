#include <stdio.h>

#include "../include/debug.h"
#include "../include/list.h"

static void print_int(FILE* fp, void* data)
{
    fprintf(fp, "%d", *(int*)data);
}

int main()
{
    struct List list = {};
    if (ListCtor(&list, sizeof(int)) != 0) {
        fprintf(stderr, "list calloc failed\n");
        return LC_BAD_ALLOC;
    }

    LIST_DUMP(&list, "newly created list", print_int);

    const int elems[] = {90, 80, -10, -20, 100, -98, -1, 666};

    ListInsertAtHead(&list, elems);
    LIST_DUMP(&list, "first head insertion", print_int);
    ListInsertAtTail(&list, elems + 1);
    LIST_DUMP(&list, "first tail insertion", print_int);
    ListInsertAtHead(&list, elems + 2);
    ListInsertAtHead(&list, elems + 3);
    ListDeleteFromTail(&list);
    ListDeleteFromTail(&list);
    ListInsertAfter(&list, elems + 4, 4);
    ListInsertAtHead(&list, elems + 5);
    ListInsertBefore(&list, elems + 6, 1);
    ListDeleteBefore(&list, 3);
    ListDeleteBefore(&list, 4);
    ListInsertAtTail(&list, elems + 7);
    ListDeleteAfter(&list, 4);
    LIST_DUMP(&list, "...", print_int);

    int head = 1000;
    for (size_t j = 0; j < 100; j++) {
        ListInsertAtHead(&list, &head);
    }
    LIST_DUMP(&list, "insertted 100 elements to head in the loop", print_int);
    int tail = -1000;
    for (size_t j = 0; j < 10; j++) {
        ListInsertAtTail(&list, &tail);
    }
    LIST_DUMP(&list, "inserted 10 elements to the tail in the loop", print_int);
    ListDtor(&list);
    return 0;
}
