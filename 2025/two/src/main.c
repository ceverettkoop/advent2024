#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../../include/error.h"

#define MAX_NUM_LEN 20
#define MAX_FACTORS 100
#define CLEAR_NUM_BUFFER                \
    memset(num_buf, '\0', MAX_NUM_LEN); \
    num_pos = 0;

bool is_invalid(size_t input) {
    size_t digit_ct = 0;
    size_t quotient = input;
    size_t half;
    size_t factors[MAX_FACTORS];
    size_t factor_n;
    size_t *values = NULL;

    // get digit ct
    do {
        digit_ct++;
        quotient = quotient / 10;
    } while (quotient > 0);

    /* PART ONE SOLUTION
    // odd digit ct = early exit
    if (digit_ct % 2) return false;

    // split to two values
    unsigned split_pt = (unsigned)pow(10, digit_ct / 2);
    val_b = input % split_pt;
    val_a = (input - val_b) / split_pt;

    if (val_a == val_b) {
        // printf("INFO: Invalid ID found %zu\n", input);
        return true;
    }
    */

    /*PART TWO SOLUTION*/
    // list factors
    factor_n = 0;
    half = digit_ct / 2;
    for (size_t i = 1; i < digit_ct; i++) {
        if (i > half) break;
        if (digit_ct % i == 0) {
            factors[factor_n] = i;
            factor_n++;
        }
    }

    for (size_t i = 0; i < factor_n; i++) {
        // split to n sections of val_len digits
        size_t val_len = factors[i];
        size_t n = digit_ct / val_len;
        size_t split_pt = (unsigned)pow(10, val_len);
        size_t truncated_input = input;

        values = malloc(sizeof(size_t) * n);
        check_malloc(values);

        // iterating through patterns within this factor of the input
        for (size_t k = 0; k < n; k++) {
            values[k] = truncated_input % split_pt;
            truncated_input = (truncated_input - values[k]) / split_pt;
            if (k > 0) {
                // pattern not same as prev = valid factor
                if (values[k] != values[k - 1]) {
                    goto FACTOR_IS_VALID;
                }
            }
        }
        // if we made it through prev loop without goto valid factor, this ID is invalid
        free(values);
        printf("Invalid id found: %zu\n", input);
        return true;

    FACTOR_IS_VALID:
        free(values);
        continue;
    }
    return false;
}

int main(int argc, char const *argv[]) {
    char c = 0;
    const char path[] = "./input";
    size_t range_start = 0;
    size_t range_end = 0;
    size_t sum = 0;
    size_t num_pos = 0;
    char num_buf[MAX_NUM_LEN] = {'\0'};

    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    while (c != EOF) {
        c = getc(fp);
        // hyphen means log start
        if (c == '-') {
            range_start = atol(num_buf);
            CLEAR_NUM_BUFFER;
            continue;
        }
        // digit means read number
        if (isdigit(c)) {
            num_buf[num_pos] = c;
            num_pos++;
            continue;
        }
        // comma means log end and parse now
        if (c == ',' || c == '\n') {
            range_end = atol(num_buf);
            CLEAR_NUM_BUFFER;
            // parsing
            for (size_t i = range_start; i <= range_end; i++) {
                if (is_invalid(i)) {
                    sum += i;
                }
            }
        }
    }
    printf("Answer is %zu\n", sum);

    return 0;
}
