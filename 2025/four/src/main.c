#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../../include/error.h"

#define MAX_COLS 250
#define MAX_ROWS 250

#define DIR_COUNT 8
#define DIR_NW 0
#define DIR_N 1
#define DIR_NE 2
#define DIR_E 3
#define DIR_SE 4
#define DIR_S 5
#define DIR_SW 6
#define DIR_W 7

// globals
size_t row_ct = 0;
size_t col_ct = 0;
char puzzle[MAX_ROWS][MAX_COLS] = {'\0'};

bool pos_is_valid(int row, int col) {
    if (row < 0) return false;
    if (col < 0) return false;
    if (row >= row_ct) return false;
    if (col >= col_ct) return false;
    return true;
}

void adjust_position(size_t direction, int *adj_row, int *adj_col, size_t row, size_t col) {
    switch (direction) {
        case DIR_NW:
            *adj_row = row - 1;
            *adj_col = col - 1;
            break;
        case DIR_N:
            *adj_row = row - 1;
            *adj_col = col;
            break;
        case DIR_NE:
            *adj_row = row - 1;
            *adj_col = col + 1;
            break;
        case DIR_E:
            *adj_row = row;
            *adj_col = col + 1;
            break;
        case DIR_SE:
            *adj_row = row + 1;
            *adj_col = col + 1;
            break;
        case DIR_S:
            *adj_row = row + 1;
            *adj_col = col;
            break;
        case DIR_SW:
            *adj_row = row + 1;
            *adj_col = col - 1;
            break;
        case DIR_W:
            *adj_row = row;
            *adj_col = col - 1;
            break;
        default:
            fatal_err("Unreachable\n");
            break;
    }
}

bool roll_is_valid(size_t row, size_t col) {
    size_t adj_rolls = 0;
    int adj_row = 0;
    int adj_col = 0;

    for (size_t i = 0; i < DIR_COUNT; i++) {
        adjust_position(i, &adj_row, &adj_col, row, col);

        if (pos_is_valid(adj_row, adj_col)) {
            if (puzzle[adj_row][adj_col] == '@') {
                adj_rolls++;
            }
        }
    }

    return (adj_rolls < 4);
}

int main(int argc, char const *argv[]) {
    char c = 0;
    const char path[] = "./input";

    size_t cur_col = 0;
    size_t cur_row = 0;
    size_t valid_rolls = 0;

    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // read puzzle
    while (c != EOF) {
        c = getc(fp);
        if (c == '\n') {
            col_ct = cur_col;
            cur_row++;
            cur_col = 0;
        } else {
            puzzle[cur_row][cur_col] = c;
            cur_col++;
        }
    }
    row_ct = cur_row;
    fclose(fp);

    // for each roll found, check if valid
    cur_col = 0;
    cur_row = 0;
    while (cur_row < row_ct) {
        while (cur_col < col_ct) {
            c = puzzle[cur_row][cur_col];
            if (c == '@') {
                if (roll_is_valid(cur_row, cur_col)) {
                    valid_rolls++;
                }
            }
            cur_col++;
        }
        cur_col = 0;
        cur_row++;
    }

    printf("Answer is %zu\n", valid_rolls);
    return 0;
}
