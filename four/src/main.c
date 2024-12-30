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

enum { RIGHT = 0, DOWN = 1, DOWN_AND_RIGHT = 2 };
#define DIR_COUNT 3

// globals
size_t row_ct = 0;
size_t col_ct = 0;
const char tokens[TOKEN_COUNT][TOKEN_LEN] = {"XMAS", "SMAX"};

static bool is_token(int *indices, char *puzzle) {
    for (size_t i = 0; i < TOKEN_LEN; i++) {
        int index = indices[i];
        if (index == -1) return false;
        if (index == tokens[FORWARDS][i]) continue;
        if (index == tokens[BACKWARDS][i]) continue;
        return false;
    }
    return true;
}

static int get_adj_index(size_t index, int dir, int distance) {
    if (distance == 0) return index;

    int cur_col = (index + col_ct) % col_ct;
    int cur_row = (int)(index / col_ct);
    int new_index = -1;

    switch (dir) {
        case RIGHT:
            new_index = index + distance;
            break;
        case DOWN:
            new_index = index + (col_ct * distance);
            break;
        case DOWN_AND_RIGHT:
            new_index = index + (col_ct * distance) + distance;
            break;
        default:
            fatal_err("unreachable\n");
            break;
    }

    // return -1 on overflow to next row
    int new_col = (new_index + col_ct) % col_ct;
    int new_row = (int)(new_index / col_ct);

    if (cur_col != new_col) return -1;
    if (cur_row != new_row) return -1;

    return new_index;
}

static void find_new_instances(unsigned *xmas_ct, int **instances, char *puzzle, int index) {
    int matches_found[MAX_INSTANCE_PER_CELL][TOKEN_LEN] = {{-1}};
    int test_indices[TOKEN_LEN] = {-1};
    bool already_found[MAX_INSTANCE_PER_CELL] = {false};

    size_t found_ct = 0;

    // RIGHT, DOWN, DOWN_LEFT
    for (int dir = 0; dir < DIR_COUNT; dir++) {
        for (size_t j = 0; j < TOKEN_LEN; j++) {
            test_indices[j] = get_adj_index(index, RIGHT, j);
        }
        if (is_token(test_indices, puzzle)) {
            memcpy(&(matches_found[0][found_ct]), test_indices, sizeof(int) * TOKEN_LEN);
            found_ct++;
        }
    }

    // check against prev found and if not, add to instances and tick up
    for (size_t i = 0; i < found_ct; i++) {
        for (size_t j = 0; j < *xmas_ct; j++) {
            if (!memcmp(instances[j], matches_found[i], (sizeof(int) * TOKEN_LEN))) {
                already_found[i] = true;
            }
        }
        if (!already_found[i]) {
            memcpy(instances[*xmas_ct], matches_found[i], (sizeof(int) * TOKEN_LEN));
            (*xmas_ct)++;
        }
    }
}

int main(int argc, char const *argv[]) {
    const char path[] = "./input";
    int c = 0;
    size_t matrix_sz = 0;
    unsigned xmas_ct = 0;
    size_t max_instances = 0;
    int **instances = NULL;
    char *puzzle = NULL;

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // get column count and row count; assume every column same # rows
    while (c != EOF) {
        c = fgetc(fp);
        if (c != '\n')
            col_ct++;
        else
            row_ct++;
    }

    matrix_sz = col_ct * row_ct;
    max_instances = matrix_sz;  // think this is true? too lazy to check

    // allocation
    puzzle = malloc(sizeof(char) * matrix_sz);
    check_malloc(puzzle);
    instances = malloc(sizeof(int *) * max_instances);
    check_malloc(instances);
    for (size_t i = 0; i < max_instances; i++) {
        instances[i] = malloc(sizeof(int) * 4);
        check_malloc(instances[i]);
    }

    fseek(fp, 0, SEEK_SET);
    c = 0;

    for (size_t i = 0; i < matrix_sz; i++) {
        c = fgetc(fp);
        if (c != '\n') puzzle[i] = c;
    }

    for (int i = 0; i < matrix_sz; i++) {
        find_new_instances(&xmas_ct, instances, puzzle, i);
    }

    printf("Unique occurances of XMAS is %u\n", xmas_ct);

    // cleanup
    free(puzzle);
    for (size_t i = 0; i < max_instances; i++) {
        free(instances[i]);
    }
    free(instances);
    fclose(fp);
    return 0;
}
