#ifndef LIST_DEBUG_H_
#define LIST_DEBUG_H_

#include <stddef.h>

void ListDump(const struct List *list, const char *file, const char *func,
              size_t line, const char *message);

#ifndef NDEBUG
#define LIST_DUMP(list, message) ListDump(list, __FILE__, __func__, __LINE__,\
                                          message);
#else
#define LIST_DUMP(list, message)
#endif

#define LIST_ASSERT(list) assert(ListVerify(list) == LIST_OK)

#endif
