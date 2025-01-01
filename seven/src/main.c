#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"

const char path[] = "./phonyinput";

#define MAX_DIGITS 16
#define MAX_INPUTS 16
#define MAX_EQUATIONS 1024

typedef struct Equation_tag {
        int result;
        int inputs[MAX_INPUTS];
        size_t input_ct;
} Equation;

typedef struct Operation_tag{
    int a;
    int b;
    int operator;
} Operation;

enum { ADDING = 0, MULTIPLYING = 1 };

static int solve(Operation input){
    if(input.operator == ADDING){
        return input.a + input.b;
    }else{
        return input.a * input.b;
    }
}

static bool can_be_true(const Equation *eq) {
    size_t operator_ct = eq->input_ct - 1;
    size_t word_size = (operator_ct * operator_ct);
    size_t combinations = word_size - 1;

    for (size_t i = 0; i < combinations; i++) {
        


    }



}

int main(int argc, char const *argv[]) {
    Equation equations[MAX_EQUATIONS];
    size_t equation_ct = 0;
    int c = 0;
    char number_buffer[MAX_DIGITS] = {'\0'};
    size_t buffer_index = 0;
    size_t input_index = 0;
    unsigned part_one_result = 0;

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

    printf("Part one answer is %u\n", part_one_result);

    return 0;
}
