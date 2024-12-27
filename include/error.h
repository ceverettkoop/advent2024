#ifndef ERROR_H
#define ERROR_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

void fatal_err(char *msg);
void check_malloc(void *ptr);

void fatal_err(char *msg) {
    fprintf(stderr, "FATAL ERROR: %s\n", msg);
    exit(1);
}

void check_malloc(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
}

#endif /* ERROR_H */
