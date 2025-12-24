#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../../include/error.h"

#define MAX_NUM_LEN 20
#define CLEAR_NUM_BUFFER                \
    memset(num_buf, '\0', MAX_NUM_LEN); \
    num_pos = 0;

bool is_invalid(size_t input) {
    size_t digit_ct = 0;
    size_t quotient = input;
    size_t val_a;
    size_t val_b;

    // get digit ct
    do {
        digit_ct++;
        quotient = quotient / 10;
    } while (quotient > 0);

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
    return false;
}

int main(int argc, char const *argv[]) {
    char c = 0;
    const char path[] = "./input";
    size_t range_start = 0;
    size_t range_end = 0;
    size_t part_a_sum = 0;
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
        if (isnumber(c)) {
            num_buf[num_pos] = c;
            num_pos++;
            continue;
        }
        // comma means log end and parse now
        if (c == ',') {
            range_end = atol(num_buf);
            CLEAR_NUM_BUFFER;
            // parsing
            for (size_t i = range_start; i <= range_end; i++) {
                if (is_invalid(i)) {
                    part_a_sum += i;
                    // printf("INFO: Running total is %zu\n", part_a_sum);
                }
            }
        }
    }
    printf("Part A answer is %zu\n", part_a_sum);

    return 0;
}
