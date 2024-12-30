#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"

#define TOKEN_COUNT 2
#define TOKEN_LEN 4
#define MAX_INSTANCE_PER_CELL 4
#define FORWARDS 0
#define BACKWARDS 1

// we go these directions and check for backwards, so that should be everything?
enum { RIGHT = 0, DOWN = 1, DOWN_AND_RIGHT = 2, UP_AND_RIGHT = 3 };

#define DIR_COUNT 4

// globals
size_t row_ct = 0;
size_t col_ct = 0;
size_t matrix_sz = 0;

const char tokens[TOKEN_COUNT][TOKEN_LEN] = {"XMAS", "SAMX"};

static bool is_token(int *indices, char *puzzle) {
    bool forwards = true;
    bool backwards = true;
    for (size_t i = 0; i < TOKEN_LEN; i++) {
        int index = indices[i];
        if (index == -1) return false;
        char value = puzzle[index];
        if (value != tokens[FORWARDS][i]) forwards = false;
        if (value != tokens[BACKWARDS][i]) backwards = false;
        if (!forwards && !backwards) return false;
    }
    return true;
}

static int get_adj_index(size_t index, int dir, int distance) {
    if (distance == 0) return index;

    int cur_row = (int)(index / col_ct);
    int new_index = -1;

    switch (dir) {
        case RIGHT:
            new_index = index + distance;
            // return -1 on overflow to next row
            if (cur_row != (int)(new_index / col_ct)) return -1;
            break;
        case DOWN:
            new_index = index + (col_ct * distance);
            if (new_index >= matrix_sz) return -1;
            break;
        case DOWN_AND_RIGHT:
            new_index = index + (col_ct * distance) + distance;
            if (new_index >= matrix_sz) return -1;
            if ((int)(new_index / col_ct) - cur_row > 1) return -1;  // overflow
            break;
        case UP_AND_RIGHT:
            new_index = index - (col_ct * distance) + distance;
            if (new_index < 0) return -1;
            if (cur_row - (int)(new_index / col_ct) < 1) return -1;  // overflow
            break;
        default:
            fatal_err("unreachable\n");
            break;
    }

    return new_index;
}

static void find_new_instances(unsigned *xmas_ct, char *puzzle, int index) {
    int test_indices[TOKEN_LEN] = {-1};
    for (int dir = 0; dir < DIR_COUNT; dir++) {
        for (size_t j = 0; j < TOKEN_LEN; j++) {
            test_indices[j] = get_adj_index(index, dir, j);
        }
        if (is_token(test_indices, puzzle)) {
            (*xmas_ct)++;
        }
    }
}

int main(int argc, char const *argv[]) {
    const char path[] = "./input";
    int c = 0;
    unsigned xmas_ct = 0;
    char *puzzle = NULL;

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // get column count and row count; assume every column same # rows
    bool col_found = false;
    while (c != EOF) {
        c = fgetc(fp);
        if (c != '\n') {
            if (!col_found) col_ct++;
        } else {
            col_found = true;
            row_ct++;
        }
    }

    matrix_sz = col_ct * row_ct;

    // allocation
    puzzle = malloc(sizeof(char) * matrix_sz);
    check_malloc(puzzle);

    fseek(fp, 0, SEEK_SET);
    c = 0;

    for (size_t i = 0; i < matrix_sz; i++) {
        c = fgetc(fp);
        if (c != '\n'){
            puzzle[i] = c;
        }else{
            --i; //lol sorry
        }
    }

    for (int i = 0; i < matrix_sz; i++) {
        find_new_instances(&xmas_ct, puzzle, i);
    }

    printf("Unique occurances of XMAS is %u\n", xmas_ct);

    // cleanup
    free(puzzle);
    fclose(fp);
    return 0;
}
