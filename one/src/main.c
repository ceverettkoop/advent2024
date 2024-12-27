#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "error.h"

#define MAX_DIGITS 8

static int cmp(const void *a, const void *b) {
    return *(int *)(a) > *(int *)(b);
}

int main(int argc, char const *argv[]) {
    const char path[] = "./input";
    size_t input_ct = 0;
    size_t cur_row = 0;
    size_t a_pos = 0;
    size_t b_pos = 0;
    // part one
    unsigned result = 0;
    unsigned difference = 0;
    unsigned similarity_score = 0;
    // part two
    int *column_a = NULL;
    int *column_b = NULL;
    int c = 0;
    char a_buf[MAX_DIGITS] = {'\0'};
    char b_buf[MAX_DIGITS] = {'\0'};
    bool writing_to_a = true;

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // count first lol
    while (c != EOF) {
        c = fgetc(fp);
        if (c == '\n') input_ct++;
    }

    // allocation
    column_a = malloc(sizeof(int) * input_ct);
    check_malloc(column_a);
    column_b = malloc(sizeof(int) * input_ct);
    check_malloc(column_b);

    fseek(fp, 0, SEEK_SET);
    c = 0;

    while (c != EOF) {
        c = fgetc(fp);
        switch (c) {
            case ' ':
                writing_to_a = !writing_to_a;
                break;
            case '\n':
                writing_to_a = !writing_to_a;
                column_a[cur_row] = atoi(a_buf);
                column_b[cur_row] = atoi(b_buf);
                a_buf[0] = '\0';
                b_buf[0] = '\0';
                a_pos = 0;
                b_pos = 0;
                cur_row++;
                break;
            default:
                if (writing_to_a) {
                    if (a_pos == MAX_DIGITS) fatal_err("INPUT VALUE OVERFLOW");
                    a_buf[a_pos] = c;
                    a_pos++;
                } else {
                    if (b_pos == MAX_DIGITS) fatal_err("INPUT VALUE OVERFLOW");
                    b_buf[b_pos] = c;
                    b_pos++;
                }
                break;
        }
    }

    qsort(column_a, input_ct, sizeof(int), cmp);
    qsort(column_b, input_ct, sizeof(int), cmp);

    // stupid nested loop to find occurance of a in b
    for (size_t i = 0; i < input_ct; i++) {
        int value = column_a[i];
        size_t col_b_freq = 0;
        unsigned product = 0;
        for (size_t j = 0; j < input_ct; j++) {
            if (value == column_b[j]) col_b_freq++;
        }
        product = value * col_b_freq;
        similarity_score += product;
        // printf("INPUT ONE: %d, INPUT TWO: %d\n", column_a[i], column_b[i]);
        // printf("PRODUCT IS %u, SCORE IS NOW %u\n", product, similarity_score);
    }

    // totalling up for part one
    for (size_t i = 0; i < input_ct; i++) {
        // printf("INPUT ONE: %d, INPUT TWO: %d\n", column_a[i], column_b[i]);
        difference = abs(column_a[i] - column_b[i]);
        result += difference;
        // printf("DIFFERENCE IS %u, TOTAL IS NOW %u\n", difference, result);
    }

    printf("Part one answer is %u, part two answer is %u\n", result, similarity_score);
    free(column_a);
    free(column_b);
    return 0;
}
