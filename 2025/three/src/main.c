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

//for part one instead of part two set this to 2
#define JOLTAGE_DIGIT_CT 12

int main(int argc, char const *argv[]) {
    char c = 0;
    const char path[] = "./input";
    char banks[ROW_MAX][COL_MAX] = {'\0'};
    size_t cur_row = 0;
    size_t cur_col = 0;
    size_t rows_read = 0;
    size_t col_ct = 0;
    size_t sum = 0;
    size_t largest_joltage = 0;
    char joltage_digits[JOLTAGE_DIGIT_CT] = {0};

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
        size_t digits_found = 0;

        for (size_t i = 0; i < JOLTAGE_DIGIT_CT; i++) {
            highest_val = '0';
            size_t col_pos = digits_found == 0 ? 0 : highest_from_left_index + 1;
            while (col_pos < col_ct - JOLTAGE_DIGIT_CT + 1 + digits_found) {
                if (banks[row_pos][col_pos] > highest_val) {
                    highest_val = banks[row_pos][col_pos];
                    highest_from_left_index = col_pos;
                }
                col_pos++;
            }
            joltage_digits[digits_found] = banks[row_pos][highest_from_left_index];
            digits_found++;
        }

        for (size_t i = 0; i < JOLTAGE_DIGIT_CT; i++) {
            largest_joltage += (joltage_digits[i] - 48) * pow(10, JOLTAGE_DIGIT_CT - i - 1);
        }

        printf("Largest found is %zu\n", largest_joltage);
        sum += largest_joltage;
        largest_joltage = 0;
    }

    printf("Answer is %zu\n", sum);
    return 0;
}
