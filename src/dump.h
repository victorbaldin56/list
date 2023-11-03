#ifndef LIST_DUMP_H_
#define LIST_DUMP_H_

#include <stdio.h>

FILE *HTCreateLog(void);

void HTDumpArray(FILE *stream, int *array, size_t size);

#endif // LIST_DUMP_H_
