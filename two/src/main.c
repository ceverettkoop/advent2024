#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../../include/error.h"

#define MAX_DIGITS 8

static size_t generate_alternatives(int ***out_ptr, int *report, size_t max_levels){
    size_t level_ct = 0;
    for (size_t i = 0; i < max_levels; i++){
        if(report[i] != 0) level_ct++;
    }
    int **alternatives = malloc(sizeof(int*) * (level_ct));
    check_malloc(alternatives);
    for (size_t i = 0; i < level_ct; i++){
        alternatives[i] = malloc(sizeof(int) * max_levels);
        check_malloc(alternatives[i]);
        memcpy(alternatives[i], report, sizeof(int) * max_levels);
        //rm level at index i and move those after up
        memcpy(&(alternatives[i][i]), &(alternatives[i][i + 1]), sizeof(int) * (max_levels - i));
        alternatives[i][max_levels] = 0;
    }
    *out_ptr = alternatives;
    return level_ct;
}

static bool level_is_safe(int *level, size_t max_inputs, bool first_run) {
    bool is_inc = true;
    bool is_dec = true;

    for (size_t i = 0; i < max_inputs; i++) {
        int value = level[i];
        if (value == 0) break;  // early exit if value is 0 means we are done
        // check slope except for on first member
        if (i > 0) {
            int prev_value = level[i - 1];
            int slope = value - prev_value;
            // rate is ok check
            if (abs(slope) > 3) goto FAILURE;
            // asc or desc check
            is_inc = (slope > 0 && is_inc);
            is_dec = (slope < 0 && is_dec);
            if (!is_dec && !is_inc) goto FAILURE;
        }
    }
    return true;  // assume ok if not failed earlier

//on 
FAILURE:
    if (!first_run) return false;
    //first run only scenario:
    int **alternatives = NULL;
    size_t alternatives_ct = generate_alternatives(&alternatives, level, max_inputs);
    bool alternative_success = false;
    for (size_t i = 0; i < alternatives_ct; i++){
        if(level_is_safe(alternatives[i], max_inputs, false)){
            alternative_success = true;
            break;
        }
    }
//cleanup
    for (size_t i = 0; i < alternatives_ct; i++){
        free(alternatives[i]);
    }
    free(alternatives);
    return alternative_success;
}

int main(int argc, char const *argv[]) {
    const char path[] = "./input";
    size_t input_ct = 0;
    size_t max_inputs = 0;
    size_t cur_row = 0;
    size_t digit_pos = 0;
    size_t level_index = 0;
    size_t inputs_this_line = 1;
    int c = 0;
    char level_buffer[MAX_DIGITS] = {'\0'};
    int **values = NULL;

    size_t levels_safe_ct = 0;

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // count inputs and determine max input length

    while (c != EOF) {
        c = fgetc(fp);
        if (c == ' ') inputs_this_line++;
        if (c == '\n') {
            input_ct++;
            if (inputs_this_line > max_inputs) max_inputs = inputs_this_line;
            inputs_this_line = 1;
        }
    }

    // allocation
    values = malloc(sizeof(int *) * input_ct);
    check_malloc(values);
    for (size_t i = 0; i < input_ct; i++) {
        values[i] = calloc(max_inputs, sizeof(int));
        check_malloc(values[i]);
    }

    // read
    fseek(fp, 0, SEEK_SET);
    c = 0;
    while (c != EOF) {
        c = fgetc(fp);

        switch (c) {
            case EOF:
                break;
            case '\n':
                values[cur_row][level_index] = atoi(level_buffer);
                memset(level_buffer,'\0',sizeof(char) * 8);
                cur_row++;
                digit_pos = 0;
                level_index = 0;
                break;
            case ' ':
                digit_pos = 0;
                values[cur_row][level_index] = atoi(level_buffer);
                memset(level_buffer,'\0',sizeof(char) * 8);
                level_index++;
                break;
            default:
                level_buffer[digit_pos] = c;
                digit_pos++;
                break;
        }
    }

    for (size_t i = 0; i < input_ct; i++) {
        printf("Report #%lu is: ", (i + 1));
        for (size_t j = 0; j < max_inputs; j++) {
            int value = values[i][j];
            if (value) printf("%d ", value);
        }
        printf("\n");

        if (level_is_safe(values[i], max_inputs, true)) {
            levels_safe_ct++;
            printf("IS SAFE\n");
        } else
            printf("UNSAFE\n");
    }

    printf("Part TWO answer, ct of levels safe after correct is: %lu\n", levels_safe_ct);

    // cleanup
    for (size_t i = 0; i < input_ct; i++) {
        free(values[i]);
    }
    free(values);

    return 0;
}