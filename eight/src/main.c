#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "../../include/error.h"

#define MAX_OCCURANCES 1024
#define OUT_OF_BOUNDS -1
#define PART_TWO

typedef struct Position_tag {
        int x;
        int y;
} Position;

typedef struct AntennaSet_tag{
    size_t ct;
    Position pos[MAX_OCCURANCES];
}AntennaSet;

// globals
const char path[] = "./input";
size_t row_ct = 0;
size_t col_ct = 0;
size_t matrix_sz = 0;
AntennaSet antennas[UCHAR_MAX];
bool *loc_found = NULL;

static int index_from_pos(Position pos) {
    int ret = 0;
    ret += pos.x;
    ret += pos.y * col_ct;
    if(pos.x < 0) return OUT_OF_BOUNDS;
    if(pos.y < 0) return OUT_OF_BOUNDS;
    if(pos.x >= col_ct) return OUT_OF_BOUNDS;
    if(pos.y >= row_ct) return OUT_OF_BOUNDS;
    return ret;
}

static Position pos_from_index(int index) {
    Position ret;
    ret.x = (index + col_ct) % col_ct;
    ret.y = index / col_ct;
    return ret;
}

static bool point_in_line_with_pair(int index, Position a, Position b){
    Position c = pos_from_index(index);
    if(c.x == a.x){
        if(c.y == b.y) return true;
    }
    if(c.y == a.y){
        
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
            if (isalnum(c)){
                Position pos = pos_from_index(i);
                antennas[c].pos[antennas[c].ct] = pos;
                (antennas[c].ct)++;
            }
            puzzle[i] = c;
            i++;
        }
    }
    fclose(fp);
    return puzzle;
}

static int mark_valid_antinodes_for_pair(Position pos_a, Position pos_b){
    int x_diff = pos_a.x - pos_b.x;
    int y_diff = pos_a.y - pos_b.y;
    int result = 0;
    Position antinode_a = pos_a;
    Position antinode_b = pos_b;

    antinode_a.x += x_diff;
    antinode_b.x -= x_diff;

    antinode_a.y += y_diff;
    antinode_b.y -= y_diff;

    printf("Antinodes are %d x %d and %d x %d\n", antinode_a.x, antinode_a.y, antinode_b.x, antinode_b.y);
    result += (index_from_pos(antinode_a) != OUT_OF_BOUNDS) + (index_from_pos(antinode_b) != OUT_OF_BOUNDS);
    printf("%d antinodes are in bounds\n", result);
    if(index_from_pos(antinode_a) != OUT_OF_BOUNDS) loc_found[index_from_pos(antinode_a)] = true;
    if(index_from_pos(antinode_b) != OUT_OF_BOUNDS) loc_found[index_from_pos(antinode_b)] = true;
#ifdef PART_TWO
    //for given pair find points in line with each
    for (size_t i = 0; i < matrix_sz; i++){
        if(point_in_line_with_pair(i, pos_a, pos_b)){
            result++;
        }
    }
#endif
    return result;
}

static int mark_all_antinodes_for_type(char *puzzle, AntennaSet *target){
    int antinodes_found = 0;
    //for each pair of characters
    for (size_t a = 0; a < target->ct; a++){
        Position loc_a = target->pos[a];
        for (size_t b = a + 1; b < target->ct; b++){
            Position loc_b = target->pos[b];
            printf("Testing pos %d x %d and %d x %d\n", loc_a.x, loc_a.y, loc_b.x, loc_b.y);
            antinodes_found += mark_valid_antinodes_for_pair(loc_a, loc_b);
        }
    }
    return antinodes_found;
}

int main(int argc, char const *argv[]) {
    char *puzzle = NULL;
    unsigned result = 0;
    //set antennas of each type to 0 found
    for (size_t i = 0; i < UCHAR_MAX; i++){
        antennas[i].ct = 0;
    }
    puzzle = parse_puzzle();
    loc_found = malloc(sizeof(bool) * matrix_sz);
    memset(loc_found, false, sizeof(bool) * matrix_sz);
    for (int i = 0; i < UCHAR_MAX; i++){
        if(antennas[i].ct){
            printf("For char %c\n", (char)i);
            mark_all_antinodes_for_type(puzzle, &(antennas[i]));
        }
    }
    for (size_t i = 0; i < matrix_sz; i++){
        if(loc_found[i]) result++;
    }
    printf("Result is %u\n", result);
    // cleanup
    free(puzzle);
    free(loc_found);
    return 0;
}
