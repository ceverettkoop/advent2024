#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../../include/error.h"

#define COL_MAX 150
#define ROW_MAX 250

int main(int argc, char const *argv[]) {
    char c = 0;
    const char path[] = "./input";
    char banks[ROW_MAX][COL_MAX] = {'\0'};


    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    while (c != EOF) {
        c = getc(fp);
    }

    return 0;
}
