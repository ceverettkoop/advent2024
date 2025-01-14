#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "../../include/error.h"

#define OUT_OF_BOUNDS -1
#define DIR_COUNT 4
#define MAX_REGION_SZ 2048
enum { RIGHT = 0, DOWN = 1, LEFT = 2, UP = 3 };

// globals
const char path[] = "./phonyinput";
size_t row_ct = 0;
size_t col_ct = 0;
size_t matrix_sz = 0;

typedef struct Region_tag{
    int indices[MAX_REGION_SZ];
    size_t index_ct;
    struct Region_tag *prev;
    struct Region_tag *next;
}Region;

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

static void print_puzzle(char *puzzle, int cursor) {
    for (size_t i = 0; i < matrix_sz; i++) {
        if (cursor == i)
            printf("X");
        else
            printf("%c", puzzle[i]);
        if (((i + 1) % col_ct) == 0) printf("\n");
    }
    printf("\n");
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
            if (isalpha(c)) {
                puzzle[i] = c;
            } else {
                fatal_err("unexpected character in dataset\n");
            }
            i++;
        }
    }
    fclose(fp);
    return puzzle;
}

Region *append_region(char *puzzle, size_t index, Region *back){
    Region *cur = malloc(sizeof(Region));
    check_malloc(cur);
    int cursor = index;
    int value = 0;
    int adj_value = 0;
    cur->prev = back;
    cur->next = NULL;
    cur->index_ct = 1;

    while(value == adj_value){
        value = puzzle[cursor + cur->index_ct - 1];
        for (size_t dir = 0; dir < DIR_COUNT; dir++){
            int adj_index = get_adj_index(index, dir, 1);
            adj_value = puzzle[adj_index];
            if(value == adj_value){
                cursor = adj_index;
                
            }
        }
        
    }

    return cur;
}   

int main(int argc, char const *argv[]) {
    char *puzzle = parse_puzzle();
    Region *back = NULL;
    bool *fenced = malloc(sizeof(bool) * matrix_sz);
    check_malloc(fenced);
    memset(fenced, false ,sizeof(bool) * matrix_sz);

    for (size_t i = 0; i < matrix_sz; i++){
        if(fenced[i]) continue;
        back = append_region(puzzle, i, back);
        for (size_t j = 0; j < back->index_ct; j++){
            fenced[back->indices[j]] = true;
        }
    }
    
    free(puzzle);
    return 0;
}
