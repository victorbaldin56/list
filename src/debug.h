#ifndef LIST_DEBUG_H_
#define LIST_DEBUG_H_

#include <stddef.h>

void ListDump(const struct List *list, const char *file, const char *func,
              size_t line);

#endif
