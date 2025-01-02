#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>

#include "../../include/error.h"

const char path[] = "./input";

#define MAX_DIGITS 16
#define MAX_INPUTS 16
#define MAX_EQUATIONS 1024
#define MAX_INTS 16
#define MAX_RESULT_LEN 1024

typedef struct Equation_tag {
        uint64_t result;
        uint64_t inputs[MAX_INPUTS];
        size_t input_ct;
} Equation;

typedef struct Operation_tag {
        uint64_t a;
        uint64_t b;
        int operator;
} Operation;

enum { ADDING = 0, MULTIPLYING = 1, CONCACTINATING = 2 };

static void int_to_bitset(size_t n, int *bitset, size_t bit_ct, size_t base) {
    memset(bitset, 0, sizeof(int) * bit_ct);
    for (size_t i = 0; i < bit_ct; i++) {
        if (n == 0) break;
        bitset[i] = n % base;
        n = n / base;
    }
}

static uint64_t ct_digits(uint64_t input){
    uint64_t digits = 1;
    uint64_t quotient = input;
    while(quotient > 9){
        digits++;
        quotient = quotient / 10;
    }
    return digits;
}

static uint64_t solve(Operation input) {
    uint64_t factor = 0;

    switch (input.operator) {
        case ADDING:
            return input.a + input.b;
            break;
        case MULTIPLYING:
            return input.a * input.b;
            break;
        case CONCACTINATING:
            factor = pow(10, (ct_digits(input.b)));
            return (input.a * factor) + input.b;
            break;
        default:
            fatal_err("Unreachable\n");
            return 0;
            break;
    }
}

static bool equation_matches(const Equation *eq, int *operators) {
    unsigned long long sum = 0;

    for (size_t i = 0; i < (eq->input_ct - 1); i++) {
        Operation op;
        op.operator= operators[i];
        if (i == 0) {
            op.a = eq->inputs[0];
            op.b = eq->inputs[1];
        } else {
            op.a = sum;
            op.b = eq->inputs[i + 1];
        }
        sum = solve(op);
    }
    return (sum == eq->result);
}

static bool can_be_true(const Equation *eq, size_t base) {
    size_t operator_ct = eq->input_ct - 1;
    size_t combinations = pow(base, operator_ct);
    int *operator_types = malloc(sizeof(int) * operator_ct);

    for (size_t i = 0; i < combinations; i++) {
        int_to_bitset(i, operator_types, operator_ct, base);
        if (equation_matches(eq, operator_types)) return true;
    }

    free(operator_types);
    return false;
}

int main(int argc, char const *argv[]) {
    Equation equations[MAX_EQUATIONS];
    size_t equation_ct = 0;
    int c = 0;
    char number_buffer[MAX_DIGITS] = {'\0'};
    size_t buffer_index = 0;
    size_t input_index = 0;
    unsigned long long cur_total = 0;

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    while (c != EOF) {
        c = fgetc(fp);
        if (isdigit(c)) {
            number_buffer[buffer_index] = c;
            buffer_index++;
            continue;
        }
        if (c == ':') {
            equations[equation_ct].result = atoll(number_buffer);
            buffer_index = 0;
            memset(number_buffer, '\0', MAX_DIGITS);
        }
        if (c == ' ') {
            if (buffer_index == 0) continue;  // to ignore space after colon
            equations[equation_ct].inputs[input_index] = atoll(number_buffer);
            input_index++;
            buffer_index = 0;
            memset(number_buffer, '\0', MAX_DIGITS);
        }
        if (c == '\n') {
            equations[equation_ct].inputs[input_index] = atoll(number_buffer);
            equations[equation_ct].input_ct = input_index + 1;
            equation_ct++;
            input_index = 0;
            buffer_index = 0;
            memset(number_buffer, '\0', MAX_DIGITS);
        }
    }

    //part one
    for (size_t i = 0; i < equation_ct; i++) {
        if (can_be_true(&(equations[i]), 2)) {
            unsigned long long last_total = cur_total;
            cur_total += equations[i].result;
            if (cur_total < last_total) {
                printf("result overflow, total before addition was %llu\n", last_total);
                fatal_err("overflow");
            }
        }
    }
    printf("Part one answer is: %llu\n", cur_total);

    //part two
    cur_total = 0;
    for (size_t i = 0; i < equation_ct; i++) {
        if (can_be_true(&(equations[i]), 3)) {
            unsigned long long last_total = cur_total;
            cur_total += equations[i].result;
            if (cur_total < last_total) {
                printf("result overflow, total before addition was %llu\n", last_total);
                fatal_err("overflow");
            }
        }
    }
    printf("Part two answer is: %llu\n", cur_total);

    return 0;
}

// this is completely unnecessary and I wrote it because of a stupid overflow bug, but it does work so keeping it here
// for posterity
/*
static void big_number_to_string(unsigned long long *numbers, char *out_string) {
    char digit_buffer[MAX_RESULT_LEN];
    unsigned output_digits[MAX_RESULT_LEN] = {0};
    memset(out_string, '\0', MAX_RESULT_LEN);

    // idiot function to add big numbers
    int big_num_i = 0;
    while (numbers[big_num_i] != 0) {
        memset(digit_buffer, '\0', MAX_RESULT_LEN);
        // big number into digits
        sprintf(digit_buffer, "%llu", numbers[big_num_i]);
        // find least significant digit index
        size_t lsd_pos = MAX_RESULT_LEN - 1;
        while (digit_buffer[lsd_pos] == '\0') {
            lsd_pos--;
        }
        // reading digit_buffer right to left, outputting left to right ;_;
        int out_i = 0;
        for (int in_i = lsd_pos; in_i > -1; in_i--) {
            output_digits[out_i] += (digit_buffer[in_i] - '0');
            // carry the one
            if (output_digits[out_i] > 9) {
                output_digits[out_i] = output_digits[out_i] - 10;
                output_digits[out_i + 1] += 1;
            }
            out_i++;
        }
        big_num_i++;
        if (big_num_i == MAX_INTS) fatal_err("Too many integers to add\n");
    }

    // read output_digits backwards as chars
    int msd_pos = MAX_RESULT_LEN - 1;
    while (output_digits[msd_pos] == 0) {
        msd_pos--;
    }
    int i = 0;
    for (int k = msd_pos; k > -1; k--) {
        out_string[i] = output_digits[k] + '0';
        i++;
    }
}
*/