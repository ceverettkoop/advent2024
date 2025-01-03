#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "../../include/error.h"

#define OUT_OF_BOUNDS 0

//this time do it so opposite directions are opposite each other
enum { UP = -1, DOWN = 1, LEFT = -2, RIGHT = 2, UP_AND_LEFT = -3, DOWN_AND_RIGHT = 3, UP_AND_RIGHT = -4, DOWN_AND_LEFT = 4, UNALIGNED = 5};

// globals
const char path[] = "./phonyinput";
size_t row_ct = 0;
size_t col_ct = 0;
size_t matrix_sz = 0;
bool antenna_types[UCHAR_MAX] = {false};

typedef struct Position_tag{
    int x;
    int y;
}Position;

static int index_from_pos(Position pos){
    int ret = 0;
    ret += pos.x;
    ret += pos.y * col_ct;
    return ret;
}

static Position pos_from_index(int index){
    Position ret;
    ret.x = index % col_ct;
}


static int dir_from_a_to_b(int a, int b){

}


static char *parse_puzzle(){
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
            if(isalnum(c)) antenna_types[c] = true;
            puzzle[i] = c;
            i++;
        }
    }
    fclose(fp);

    return puzzle;
}

int main(int argc, char const *argv[]) {
    
    char *puzzle = parse_puzzle;


    // cleanup
    free(puzzle);

    return 0;
}
