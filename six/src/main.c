#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"


enum { RIGHT = 0, DOWN = 1, LEFT = 2, UP = 3 };

// globals
const char path[] = "./input";
size_t row_ct = 0;
size_t col_ct = 0;
size_t matrix_sz = 0;

static void turn_right(int *dir) {
    switch (*dir) {
        case UP:
            *dir = RIGHT;
            break;
        default:
            (*dir)++;
            break;
    }
}

static void log_guard_pos(int guard_pos, bool *log) {
    if (guard_pos < 0) return;
    log[guard_pos] = true;
}

// returns -1 on out of bounds request
static int get_adj_index(size_t index, int dir, int distance) {
    if (distance == 0) return index;
    int cur_row = (int)(index / col_ct);
    int new_index = -1;
    switch (dir) {
        case RIGHT:
            new_index = index + distance;
            if (cur_row != (int)(new_index / col_ct)) return -1;
            break;
        case DOWN:
            new_index = index + (col_ct * distance);
            if (new_index >= matrix_sz) return -1;
            break;
        case UP:
            new_index = index - (col_ct * distance);
            if (new_index < 0) return -1;
            break;
        case LEFT:
            new_index = index - distance;
            if (cur_row != (int)(new_index / col_ct)) return -1;
            break;
        default:
            fatal_err("unreachable\n");
            break;
    }
    return new_index;
}

static void move_guard(int *guard_pos, int *guard_dir, char *puzzle) {
    int destination = get_adj_index(*guard_pos, *guard_dir, 1);
    if (destination == -1) {  // OOB
        *guard_pos = -1;
        return;
    }
    char obstacle = puzzle[destination];
    if (obstacle == '#') {
        turn_right(guard_dir);
    } else {
        *guard_pos = destination;
    }
}

int main(int argc, char const *argv[]) {
    int c = 0;
    char *puzzle = NULL;
    int guard_pos = -1;
    bool *log = NULL;
    int *loop_log;
    unsigned part_one_result = 0;
    int guard_dir = UP;
    int max_loop_len = 0;

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
    max_loop_len = (2 * col_ct + 2 * row_ct) * 2;

    puzzle = malloc(sizeof(char) * matrix_sz);
    check_malloc(puzzle);

    log = malloc(sizeof(bool) * matrix_sz);
    check_malloc(log);
    //plenty big enough i guess
    loop_log = malloc(sizeof(int) * max_loop_len);
    check_malloc(loop_log);

    fseek(fp, 0, SEEK_SET);
    c = 0;
    size_t i = 0;
    while (i < matrix_sz) {
        c = fgetc(fp);
        if (c != '\n') {
            puzzle[i] = c;
            if (c == '^') guard_pos = i;
            i++;
        }
    }
    fclose(fp);

    while (guard_pos != -1) {
        log_guard_pos(guard_pos, log);
        move_guard(&guard_pos, &guard_dir, puzzle);
    }

    for (size_t i = 0; i < matrix_sz; i++) {
        if (log[i]) part_one_result++;
    }

    printf("Part one result is: %u\n", part_one_result);


    // cleanup
    free(puzzle);
    free(log);
    free(loop_log);

    return 0;
}
