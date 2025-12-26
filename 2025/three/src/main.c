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
    size_t cur_row = 0;
    size_t cur_col = 0;
    size_t rows_read = 0;
    size_t col_ct = 0;
    size_t sum = 0;

    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    while (c != EOF) {
        c = getc(fp);
        if (c == '\n') {
            col_ct = cur_col;
            cur_row++;
            cur_col = 0;
            rows_read++;
            continue;
        }
        banks[cur_row][cur_col] = c;
        cur_col++;
    }

    for (size_t row_pos = 0; row_pos < rows_read; row_pos++) {
        char highest_val = '0';
        size_t highest_from_left_index = 0;
        size_t highest_from_right_index = 0;

        for (int col_pos = 0; col_pos < col_ct; col_pos++) {
            if (banks[row_pos][col_pos] > highest_val) {
                // edge case if highest value is on last col, we can't use it
                if (col_pos != col_ct - 1) {
                    highest_val = banks[row_pos][col_pos];
                    highest_from_left_index = col_pos;
                }
            }
        }
        highest_val = '0';
        // find highest value from right that is past highest from left
        for (int col_pos = col_ct - 1; col_pos > highest_from_left_index; col_pos--) {
            if (banks[row_pos][col_pos] > highest_val) {
                highest_val = banks[row_pos][col_pos];
                highest_from_right_index = col_pos;
            }
        }

        size_t largest_joltage =
            ((banks[row_pos][highest_from_left_index] - 48) * 10) + (banks[row_pos][highest_from_right_index] - 48);
        printf("Largest found is %zu\n", largest_joltage);
        sum += largest_joltage;
    }

    printf("Answer is %zu\n", sum);
    return 0;
}
