#include "recalloc.h"

#include <stdlib.h>
#include <string.h>

void *__recalloc(void *ptr, size_t membsize, size_t nmemb)
{
    if (!ptr)
        return NULL;

    void *newptr = calloc(membsize, nmemb);
    if (!newptr)
        return NULL;
    memcpy(newptr, ptr, membsize * nmemb);
    free(ptr);
    return newptr;
}
