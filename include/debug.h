#ifndef LIST_DEBUG_H_
#define LIST_DEBUG_H_

#include <stddef.h>
#include <stdio.h>

typedef void PrintFunc(FILE* fp, void* data);

void ListDump(const struct List *list, const char *file, const char *func,
              size_t line, const char *message, PrintFunc* print_func);

#ifndef NDEBUG
#define LIST_DUMP(list, message, print_func) ListDump(list, __FILE__, __func__, __LINE__,\
                                          message, print_func);
#else
#define LIST_DUMP(list, message)
#endif

#define LIST_ASSERT(list) assert(ListVerify(list) == LIST_OK)

#endif
