#include "debug.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "list.h"

inline static FILE *CreateLogFile(char *path_to_dot_src)
{
    assert(path_to_dot_src);
    time_t now = time(NULL);
    char timestamp[FILENAME_MAX] = {};
    strftime(timestamp, FILENAME_MAX, "logs/%Y-%m-%d_%H:%M:%S",
                                      gmtime(&now));
    struct timespec ts = {};
    timespec_get(&ts, TIME_UTC);
    snprintf(path_to_dot_src, PATH_MAX, "%s.%09ld.dot",
                                        timestamp, ts.tv_nsec);
    return fopen(path_to_dot_src, "w");
}

static void DotDump(const struct List *list, char *path_to_dot);

void ListDump(const struct List *list, const char *file, const char *func,
              size_t line, const char *message)
{
    assert(list);
    assert(file && func && message);
    system("mkdir -p logs");
    FILE *fp = fopen("logs/dump.html", "a");
    if (!fp) {
        perror("ListDump");
        return;
    }
    fprintf(fp, "<h2>===============================================</br>\n"
                "===============================================</h2>\n");
    fprintf(fp, "<h2>ListDump from function %s, %s:%zu</h2>"
                "size = %zd</br>free = %zd\n",
                func, file, line, list->size, list->free);
    char src_filename[FILENAME_MAX] = {};
    DotDump(list, src_filename);
    long cmd_size = __sysconf(_SC_ARG_MAX);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    char *cmd = (char *)calloc(cmd_size, sizeof(*cmd));
    snprintf(cmd, cmd_size, "dot -T png %s -o %s.png",
                            src_filename, src_filename);
#pragma GCC diagnostic pop
    system(cmd);
    free(cmd);
    fprintf(fp, "<figure> <img src=\"%s.png\"/><figcaption>%s</br>"
                "source file: %s"
                "</figcaption></figure>",
                src_filename + sizeof("logs/") - 1, message, src_filename);
    return;
}

static void DotDump(const struct List *list, char *path_to_dot_src)
{
    assert(path_to_dot_src);
    FILE *fp = CreateLogFile(path_to_dot_src);
    if (!fp) {
        perror("DotDump");
        return;
    }
    fprintf(fp, "digraph List {\n\trankdir = LR;\n"
                "\tnode [shape = Mrecord];\n");
    fprintf(fp, "0 ");
    for (ssize_t i = 0; i < list->size; i++) {
        fprintf(fp, "-> %zd", i);
    }
    fprintf(fp, "[arrowsize = 0.0, weight = 100000, color = \"#FFFFFF\"];\n");
    for (ssize_t i = 0; i < list->size; i++) {
        fprintf(fp, "\t%zd [shape = Mrecord, "
                    "style = filled, ", i);
        if (list->prev[i] == -1)
            fprintf(fp, "fillcolor = cyan, ");
        else if (i != 0)
            fprintf(fp, "fillcolor = orange, ");
        else
            fprintf(fp, "fillcolor = red, ");

        fprintf(fp, "label = \"idx: %zd | data: %d | next: %zd | "
                    "prev: %zd\"];\n",
                    i, list->data[i], list->next[i], list->prev[i]);
    }
    for (ssize_t i = 0; i < list->size; i++) {
        fprintf(fp, "\t%zd -> %zd;\n", i, list->next[i]);
    }
    for (ssize_t i = 0; i < list->size; i++) {
        if (list->prev[i] != -1)
            fprintf(fp, "\t%zd -> %zd;\n", i, list->prev[i]);
    }
    fprintf(fp, "}\n");
    fclose(fp);
}
