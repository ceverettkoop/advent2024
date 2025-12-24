#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"

#define MAX_PAIRS 2048
#define MAX_UPDATES 2048
#define MAX_DIGITS 8
#define MAX_UPDATE_PAGES 32

const char path[] = "./input";

typedef struct Pair_tag {
        int before;
        int after;
} Pair;

static bool is_out_of_order(int precedent, int antecedent, Pair *pairs, size_t pair_ct) {
    for (size_t i = 0; i < pair_ct; i++) {
        Pair pair = pairs[i];
        if (precedent == pair.after && antecedent == pair.before) return true;
    }
    return false;
}

static void revise_update(int *input, Pair *pairs, size_t pair_ct) {
    size_t update_size = 0;
    int value = 0;
    while (value != -1) {
        update_size++;
        value = input[update_size];
    }
    for (int i = (update_size - 1); i > -1; i--) {
        int antecedent = input[i];
        // inner loop, compare everything before antecedent
        for (int j = i - 1; j > -1; j--) {
            int precedent = input[j];
            // iterate through every rule
            for (size_t k = 0; k < pair_ct; k++) {
                Pair pair = pairs[k];
                if (precedent == pair.after && antecedent == pair.before) {
                    // on every rule violation, swap them
                    input[j] = antecedent;
                    input[i] = precedent;
                    // now what we are comparing changes as well
                    int temp_a = antecedent;
                    int temp_p = precedent;
                    precedent = temp_a;
                    antecedent = temp_p;
                }
            }
        }
    }
}

static int get_middle_value(int *update) {
    size_t update_size = 0;
    int value = 0;

    while (value != -1) {
        update_size++;
        value = update[update_size];
    }
    return update[update_size / 2];
}

static bool is_valid_update(int *update, Pair *pairs, size_t pair_ct) {
    size_t update_size = 0;
    int value = 0;

    while (value != -1) {
        update_size++;
        value = update[update_size];
    }
    // start with last value, work backwards
    for (int i = (update_size - 1); i > -1; i--) {
        int antecedent = update[i];
        for (int j = i - 1; j > -1; j--) {
            int precedent = update[j];
            if (is_out_of_order(precedent, antecedent, pairs, pair_ct)) return false;
        }
    }
    return true;
}

int main(int argc, char const *argv[]) {
    int c = 0;
    int last_c = 0;
    char input_buffer[MAX_DIGITS] = {'\0'};
    size_t buffer_index = 0;
    size_t pair_index = 0;
    size_t update_index = 0;
    size_t page_index = 0;
    size_t update_ct = 0;
    size_t pair_ct = 0;
    unsigned part_one_result = 0;
    unsigned part_two_result = 0;

    // idg why this doesn't work, it is setting them to 0 not -1 but whatever
    Pair pairs[MAX_PAIRS] = {(Pair){.before = -1, .after = -1}};
    int updates[MAX_UPDATES][MAX_UPDATE_PAGES] = {{-1}};

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // parse part one
    while (true) {
        last_c = c;
        c = fgetc(fp);
        if (c == '\n' && last_c == '\n') break;  // end part one
        if (isdigit(c)) {
            input_buffer[buffer_index] = c;
            buffer_index++;
            continue;
        }
        if (c == '|') {
            pairs[pair_index].before = atoi(input_buffer);
            buffer_index = 0;
            memset(input_buffer, '\0', MAX_DIGITS);
            continue;
        }
        if (c == '\n') {
            pairs[pair_index].after = atoi(input_buffer);
            pair_index++;
            buffer_index = 0;
            memset(input_buffer, '\0', MAX_DIGITS);
            continue;
        }
    }
    pair_ct = pair_index;

    // parse part two
    while (c != EOF) {
        c = fgetc(fp);
        if (isdigit(c)) {
            input_buffer[buffer_index] = c;
            buffer_index++;
            continue;
        }
        if (c == ',') {
            updates[update_index][page_index] = atoi(input_buffer);
            buffer_index = 0;
            page_index++;
            continue;
        }
        if (c == '\n') {
            updates[update_index][page_index] = atoi(input_buffer);
            updates[update_index][page_index + 1] = -1;  // indicate end
            buffer_index = 0;
            page_index = 0;
            update_index++;
            continue;
        }
    }
    update_ct = update_index;

    fclose(fp);

    for (int i = 0; i < update_ct; i++) {
        int *update = updates[i];
        if (is_valid_update(update, pairs, pair_ct)) {
            part_one_result += get_middle_value(update);
        } else {
            revise_update(update, pairs, pair_ct);
            part_two_result += get_middle_value(update);
        }
    }

    printf("Part one total is %u\n", part_one_result);
    printf("Part two total is %u\n", part_two_result);

    // cleanup

    return 0;
}
