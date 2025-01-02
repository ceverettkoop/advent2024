#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"

const char path[] = "./input";

#define MAX_DIGITS 16
#define MAX_INPUTS 16
#define MAX_EQUATIONS 1024

typedef struct Equation_tag {
        int result;
        int inputs[MAX_INPUTS];
        size_t input_ct;
} Equation;

typedef struct Operation_tag {
        int a;
        int b;
        int operator;
} Operation;

enum { ADDING = 0, MULTIPLYING = 1 };

static void int_to_bitset(size_t n, int *bitset, size_t bit_ct) {
    memset(bitset, 0, sizeof(int) * bit_ct);
    for (size_t i = 0; i < bit_ct; i++) {
        if (n == 0) break;
        bitset[i] = n % 2;
        n = n / 2;
    }
}

static int solve(Operation input) {
    if (input.operator== ADDING) {
        return input.a + input.b;
    } else {
        return input.a * input.b;
    }
}

static bool equation_matches(const Equation *eq, int *operators) {
    int sum = 0;

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
    return sum == eq->result;
}

static bool can_be_true(const Equation *eq) {
    size_t operator_ct = eq->input_ct - 1;
    size_t word_size = operator_ct > 1 ? (1 << operator_ct) : 1;
    size_t combinations = eq->input_ct == 2 ? 2 : word_size - 1 ;
    int *operator_types = malloc(sizeof(int) * operator_ct);

    for (size_t i = 0; i < combinations; i++) {
        int_to_bitset(i, operator_types, operator_ct);
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
    unsigned long part_one_result = 0;

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
            equations[equation_ct].result = atoi(number_buffer);
            buffer_index = 0;
            memset(number_buffer, '\0', MAX_DIGITS);
        }
        if (c == ' ') {
            if (buffer_index == 0) continue;  // to ignore space after colon
            equations[equation_ct].inputs[input_index] = atoi(number_buffer);
            input_index++;
            buffer_index = 0;
            memset(number_buffer, '\0', MAX_DIGITS);
        }
        if (c == '\n') {
            equations[equation_ct].inputs[input_index] = atoi(number_buffer);
            equations[equation_ct].input_ct = input_index + 1;
            equation_ct++;
            input_index = 0;
            buffer_index = 0;
            memset(number_buffer, '\0', MAX_DIGITS);
        }
    }

    for (size_t i = 0; i < equation_ct; i++) {
        if (can_be_true(&(equations[i]))) {
            part_one_result += equations[i].result;
        }
    }

    printf("Part one answer is %lu\n", part_one_result);

    return 0;
}
