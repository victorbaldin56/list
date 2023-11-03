#include "dump.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *HTCreateLog(void)
{
    system("mkdir -p logs");
    time_t now = time(NULL);

    char filename[PATH_MAX] = {};
    strftime(filename, PATH_MAX, "logs/%Y-%m-%d_%H:%M:%S.html",
             gmtime(&now));

    return fopen(filename, "w");
}

void HTDumpArray(FILE *stream, int *array, size_t size)
{
    assert(array);
    assert(stream);

    fprintf(stream, "<table>\n");

    fprintf(stream, "<tbody>\n");
    fprintf(stream, "<tr>\n");
    for (size_t i = 0; i < size; i++) {
        fprintf(stream, "<td>%d</td>\n", array[i]);
    }
    fprintf(stream, "</tr>\n");
    fprintf(stream, "</tbody>\n");

    fprintf(stream, "</table>\n");
}
