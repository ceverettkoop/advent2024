#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "../../include/error.h"

#define INITIAL_RUN -2
#define OUT_OF_BOUNDS -1
#define DIR_COUNT 4
enum { RIGHT = 0, DOWN = 1, LEFT = 2, UP = 3 };

// globals
const char path[] = "./phonyinput";
size_t row_ct = 0;
size_t col_ct = 0;
size_t matrix_sz = 0;

static int get_adj_index(size_t index, int dir, int distance) {
    if (distance == 0) return index;
    int cur_row = (int)(index / col_ct);
    int new_index = OUT_OF_BOUNDS;
    switch (dir) {
        case RIGHT:
            new_index = index + distance;
            if (cur_row != (int)(new_index / col_ct)) return OUT_OF_BOUNDS;
            break;
        case DOWN:
            new_index = index + (col_ct * distance);
            if (new_index >= matrix_sz) return OUT_OF_BOUNDS;
            break;
        case UP:
            new_index = index - (col_ct * distance);
            if (new_index < 0) return OUT_OF_BOUNDS;
            break;
        case LEFT:
            new_index = index - distance;
            if (cur_row != (int)(new_index / col_ct)) return OUT_OF_BOUNDS;
            break;
        default:
            return OUT_OF_BOUNDS;
            break;
    }
    return new_index;
}

static void print_puzzle(int *puzzle) {
    for (size_t i = 0; i < matrix_sz; i++) {
        printf("%d", puzzle[i]);
        if (((i + 1) % col_ct) == 0) printf("\n");
    }
}

static int *parse_puzzle() {
    int *puzzle = NULL;
    int c = 0;
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");
    // get column count and row count; assume every column same # rows
    bool col_found = false;
    while (c != EOF) {
        c = fgetc(fp);
        if (c != '\n') {
            if (!col_found) col_ct++;
        } else {
            col_found = true;
            row_ct++;
        }
    }
    matrix_sz = col_ct * row_ct;
    puzzle = malloc(sizeof(int) * matrix_sz);
    check_malloc(puzzle);
    fseek(fp, 0, SEEK_SET);
    c = 0;
    size_t i = 0;
    while (i < matrix_sz) {
        c = fgetc(fp);
        if (c != '\n') {
            if (isdigit(c)) {
                puzzle[i] = c - '0';
            } else {
                fatal_err("unexpected character in dataset\n");
            }
            i++;
        }
    }
    fclose(fp);
    return puzzle;
}

static size_t get_valid_directions(int *puzzle, int pos, bool *valid_dir){
    size_t ct = 0;
    memset(valid_dir, false, sizeof(bool) * DIR_COUNT);
    for (size_t dir = 0; dir < DIR_COUNT; dir++){
        int adj_pos = get_adj_index(pos, dir, 1);
        if(adj_pos != OUT_OF_BOUNDS){
            if((puzzle[adj_pos] - 1) == puzzle[pos]){
                valid_dir[dir] = true; 
                ct++;
            }
        }
    }
    return ct;
}

static int dir_for_branch(bool* valid, int branch_index){
    size_t itr = 0;
    for (size_t dir = 0; dir < DIR_COUNT; dir++){
        if(valid[dir]){
            if(branch_index == itr) return dir;
            itr++;
        }
    }
    return OUT_OF_BOUNDS;
}

static void iterate_puzzle(int *puzzle, int pos, size_t *nines_found, int branch_index){
    //get new options
    bool valid_directions[DIR_COUNT] = {false};
    size_t branches_open = get_valid_directions(puzzle, pos, valid_directions);
    if(branches_open == 0) goto SUCCESS_CHECK;
    //move based on index passed to us from above
    if(branch_index != INITIAL_RUN){
        int dir = dir_for_branch(valid_directions, branch_index);
        pos = get_adj_index(pos, dir, 1);
    SUCCESS_CHECK:
        if(puzzle[pos] == 9){
            (*nines_found)++;
        }   
    }
    //pass adj_position down to more branches
    for (size_t i = 0; i < branches_open; i++){
        iterate_puzzle(puzzle, pos, nines_found, i);
    }
}

size_t get_trailhead_score(int *puzzle, int pos){
    size_t nines_reached = 0;
    iterate_puzzle(puzzle, pos, &nines_reached, INITIAL_RUN);
    printf("Score for trailhead at index %d is %zu\n", pos, nines_reached);
    return nines_reached;
}

int main(int argc, char const *argv[]) {
    int *puzzle = parse_puzzle();
    unsigned result = 0;

    for (size_t i = 0; i < matrix_sz; i++){
        if(puzzle[i] == 0){
            result += get_trailhead_score(puzzle, i);
        }
    }
    printf("Part one answer is %u\n", result);
    free(puzzle);
    return 0;
}
