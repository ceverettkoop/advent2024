#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "../../include/error.h"

#define LINE_MAX 1024
#define DIGIT_MAX 16

typedef struct Vector2_tag {
        unsigned x;
        unsigned y;
} Vector2;

typedef struct Machine_tag {
        Vector2 btn_a;
        Vector2 btn_b;
        Vector2 prize;
} Machine;

// globals
const char path[] = "./phonyinput";

Vector2 parse_vector(char *line) {
    size_t pos = 0;
    size_t num_pos = 0;
    Vector2 ret;
    char num[DIGIT_MAX] = {'\0'};
    while (line[pos] != 'X') {
        pos++;
    }
    // skip the X+
    pos += 2;
    while (line[pos] != ',') {
        num[num_pos] = line[pos];
        num_pos++;
        pos++;
    }
    ret.x = atoi(num);
    num_pos = 0;
    memset(num, '\0', DIGIT_MAX);
    while (line[pos] != 'Y') {
        pos++;
    }
    // skip Y+
    pos += 2;
    while (line[pos] != '\0') {
        num[num_pos] = line[pos];
        num_pos++;
        pos++;
    }
    ret.y = atoi(num);
    return ret;
}

static Machine *parse_puzzle(size_t *machine_ct) {
    char line_buffer[LINE_MAX];
    int c = 0;
    size_t line_pos = 0;
    Machine *ptr = malloc(sizeof(Machine));
    check_malloc(ptr);
    *machine_ct = 0;
    memset(line_buffer, 0, LINE_MAX * sizeof(char));

    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");
    while (c != EOF) {
        c = fgetc(fp);
        if (c != '\n') {
            line_buffer[line_pos] = c;
            line_pos++;
            if (line_pos == LINE_MAX) fatal_err("line overflow");
        } else {
            switch (line_buffer[7]) {
                case 'A':
                    ptr[*machine_ct].btn_a = parse_vector(line_buffer);
                    break;
                case 'B':
                    ptr[*machine_ct].btn_b = parse_vector(line_buffer);
                    break;
                case 'X':
                    ptr[*machine_ct].prize = parse_vector(line_buffer);
                    (*machine_ct)++;
                    ptr = realloc(ptr, sizeof(Machine) * (*machine_ct + 1));
                    check_malloc(ptr);
                    break;
                default:
                    break;
            }
            memset(line_buffer, 0, LINE_MAX * sizeof(char));
            line_pos = 0;
        }
    }
    fclose(fp);
    return ptr;
}

int main(int argc, char const *argv[]) {
    size_t ct = 0;
    Machine *machines = parse_puzzle(&ct);
    free(machines);
    return 0;
}
