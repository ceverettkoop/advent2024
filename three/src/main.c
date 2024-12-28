#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"

#define TOKEN_MAX 32

static int extract_product(const fpos_t file_pos, FILE *fp) {
    // assume provided fp is at pos
    int c = 0;
    int result = 0;
    char int_token_a[TOKEN_MAX] = {'\0'};
    char int_token_b[TOKEN_MAX] = {'\0'};
    char *cur_token = int_token_a;
    int token_pos = 0;
    // expecting token to be integer,integer)
    while (true) {
        c = fgetc(fp);
        if (c == EOF) goto EXIT;
        // if its a digit, store to current token
        if (isdigit(c)) {
            cur_token[token_pos] = c;
            token_pos++;
        } else {
            if (c == ',') {
                if (cur_token == int_token_a) {
                    cur_token = int_token_b;
                    token_pos = 0;
                } else {
                    goto EXIT;  // unexpected comma
                }
            } else if (c == ')') {
                if (cur_token == int_token_b) {
                    // we done, have presumably valid input
                    break;
                } else {
                    goto EXIT;  // unexpected )
                }
            } else {
                // invalid char
                goto EXIT;
            }
        }
        if (token_pos == TOKEN_MAX) return 0;  // overflow, assuming invalid token here
    }
    // presumably two valid numbers
    result = atoi(int_token_a) * atoi(int_token_b);
EXIT:
    fsetpos(fp, &file_pos);
    if (result == 0) printf("note we found an invalid token starting with mul at position %lld\n", file_pos);
    return result;
}

int main(int argc, char const *argv[]) {
    const char path[] = "./input";
    int c = 0;
    const char begin_token[] = "mul(";
    const char do_token[] = "do()";
    const char dont_token[] = "don't()";
    size_t begin_token_len = strlen(begin_token);
    size_t do_token_len = strlen(do_token);
    size_t dont_token_len = strlen(dont_token);
    int mul_token_index = 0;
    int do_token_index = 0;
    int dont_token_index = 0;
    fpos_t pos = 0;
    unsigned result = 0;
    bool parsing_on = true;

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // identify instances of mul(
    while (c != EOF) {
        c = fgetc(fp);
        pos++;
        // looking for mul
        if (c == begin_token[mul_token_index]) {
            mul_token_index++;
            // success, pos now = index of end
            if (mul_token_index == begin_token_len) {
                int product = extract_product(pos, fp);
                // returns 0 on failure, resets fp either way
                if (parsing_on) result += product;
                mul_token_index = 0;
            }
        } else {
            mul_token_index = 0;
        }
        // looking for do
        if (c == do_token[do_token_index]) {
            do_token_index++;
            if (do_token_index == do_token_len) {
                parsing_on = true;
                do_token_index = 0;
            }
        } else {
            do_token_index = 0;
        }
        //looking for dont
        if (c == dont_token[dont_token_index]) {
            dont_token_index++;
            if (dont_token_index == dont_token_len) {
                parsing_on = false;
                dont_token_index = 0;
            }
        } else {
            dont_token_index = 0;
        }
    }

    printf("Sum of all products is %u\n", result);
    return 0;
}
