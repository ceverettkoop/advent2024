#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "../../include/error.h"

#define MAX_OCCURANCES 1024

// globals
const char path[] = "./phonyinput";
size_t row_ct = 0;
size_t col_ct = 0;
size_t matrix_sz = 0;
bool antenna_types[UCHAR_MAX] = {false};

typedef struct Position_tag {
        int x;
        int y;
} Position;

static int index_from_pos(Position pos) {
    int ret = 0;
    ret += pos.x;
    ret += pos.y * col_ct;
    return ret;
}

static Position pos_from_index(int index) {
    Position ret;
    ret.x = (index + col_ct) % col_ct;
    ret.y = index / col_ct;
    return ret;
}

//coincident not considered aligned
static bool are_aligned(Position a_pos, Position b_pos){
    if (a_pos.x == b_pos.x) {
        if(a_pos.y == b_pos.y) return false;
        return true;
    }
    if (a_pos.y == b_pos.y){
        if(a_pos.x == b_pos.x) return false;
        return true;
    }
    //diagonal
    if(abs(a_pos.x - b_pos.x) == abs(a_pos.y - b_pos.y)){
        return true;
    }
    return false;
}

static char *parse_puzzle() {
    char *puzzle = NULL;
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
    puzzle = malloc(sizeof(char) * matrix_sz);
    check_malloc(puzzle);
    fseek(fp, 0, SEEK_SET);
    c = 0;
    size_t i = 0;
    while (i < matrix_sz) {
        c = fgetc(fp);
        if (c != '\n') {
            if (isalnum(c)) antenna_types[c] = true;
            puzzle[i] = c;
            i++;
        }
    }
    fclose(fp);
    return puzzle;
}

static int antinodes_for_pair(Position pos_a, Position pos_b){
    int x_diff = pos_a.x - pos_b.x;
    int y_diff = pos_b.y - pos_b.y;

}

static int all_antinodes_for_type(char *puzzle, int target){
    int antinodes_found = 0;
    size_t occur_ct = 0;
    int locations[MAX_OCCURANCES] = {-1};
    //find all instances of given character
    for (size_t i = 0; i < matrix_sz; i++){
        if (puzzle[i] == target){
            locations[occur_ct] = i;
            occur_ct++;
        }
    }
    //for each pair of characters
    for (size_t a = 0; a < occur_ct; a++){
        int loc_a = locations[a];
        for (size_t b = 0; b < occur_ct; b++){
            if(a == b) continue;
            int loc_b = locations[b];
            Position pos_a = pos_from_index(loc_a);
            Position pos_b = pos_from_index(loc_b);
            if(are_aligned(pos_a, pos_b)){
                //finally here we have two aligned pairs
                antinodes_found += antinodes_for_pair(pos_a, pos_b);
            }
        }
    }
    return antinodes_found;
}

int main(int argc, char const *argv[]) {
    char *puzzle = parse_puzzle;
    unsigned part_one_result = 0;
    
    for (int i = 0; i < UCHAR_MAX; i++){
        if(antenna_types[i]) part_one_result += all_antinodes_for_type(puzzle, i);
    }
    printf("Part one result is %u\n");
    // cleanup
    free(puzzle);

    return 0;
}
