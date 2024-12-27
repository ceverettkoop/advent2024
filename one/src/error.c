#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "error.h"

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
