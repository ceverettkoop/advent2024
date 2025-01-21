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
#define PART_TWO
#define MAX_EDGES 1024
#define MAX_EDGES_PER_SIDE 128

enum { RIGHT = 0, DOWN = 1, LEFT = 2, UP = 3 };

// globals
const char path[] = "./input";
size_t row_ct = 0;
size_t col_ct = 0;
size_t matrix_sz = 0;

typedef struct Region_tag {
        bool *indicies;
        struct Region_tag *prev;
        struct Region_tag *next;
} Region;

typedef struct Edge_tag {
        int dir;
        int loc;
} Edge;

typedef struct Side_tag {
        size_t edge_ct;
        int edge_indicies[MAX_EDGES_PER_SIDE];
} Side;

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

void advance(int cursor, bool *indicies, char *puzzle) {
    for (int dir = 0; dir < DIR_COUNT; dir++) {
        int value = puzzle[cursor];
        int adj_cursor = get_adj_index(cursor, dir, 1);
        // we don't want to go backwards to avoid infinite loop, i think this will still get everything?
        if (indicies[adj_cursor]) continue;
        if (puzzle[adj_cursor] == value) {
            indicies[adj_cursor] = true;
            advance(adj_cursor, indicies, puzzle);
        }
    }
}

Region *append_region(char *puzzle, size_t index, Region *back) {
    Region *cur = malloc(sizeof(Region));
    check_malloc(cur);
    cur->indicies = malloc(sizeof(bool) * matrix_sz);
    check_malloc(cur->indicies);
    memset(cur->indicies, false, sizeof(bool) * matrix_sz);
    cur->prev = back;
    if (cur->prev) cur->prev->next = cur;
    cur->next = NULL;
    cur->indicies[index] = true;
    // mark every other member of region as true
    advance(index, cur->indicies, puzzle);
    return cur;
}

void free_regions(Region *back) {
    Region *cur = back;
    while (cur->prev != NULL) {
        cur = cur->prev;
        free(cur->next->indicies);
        free(cur->next);
    }
    free(cur->indicies);
    free(cur);
}

#ifndef PART_TWO
unsigned get_perimeter(Region *cur, char *puzzle) {
    unsigned perimeter = 0;
    for (size_t i = 0; i < matrix_sz; i++) {
        if (cur->indicies[i]) {
            int value = puzzle[i];
            perimeter += 4;
            for (int dir = 0; dir < DIR_COUNT; dir++) {
                int adj_index = get_adj_index(i, dir, 1);
                if (adj_index == OUT_OF_BOUNDS) continue;
                if (value == puzzle[adj_index]) perimeter--;
            }
        }
    }
    return perimeter;
}
#endif

bool edges_are_adjacent(Edge a, Edge b){
    //horz edge, row is relevant
    if(a.dir == UP || a.dir == DOWN){
        if(b.dir == LEFT || b.dir == RIGHT) return false;
        int a_row = (int)(a.loc / col_ct);
        int b_row = (int)(b.loc / col_ct);
        //normalize down to up
        if(a.dir == DOWN) a_row--;
        if(b.dir == DOWN) b_row--;
        if(a_row == b_row){
            if(abs(a.loc - b.loc) == 1) return true;
        }else{
            return false;
        }
    }else{ //a is left or right
        if(b.dir == UP || b.dir == DOWN) return false;
        int a_col = (a.loc + col_ct) % col_ct;
        int b_col = (b.loc + col_ct) % col_ct;
        //normalize RIGHT to LEFT
        if(a.dir == RIGHT) a_col--;
        if(b.dir == RIGHT) b_col--;
        if(a_col == b_col){
            if(abs(a.loc - b.loc) == row_ct) return true;
        }else{
            return false;
        } 
    }
    return false;
}

bool edge_is_in_side(Edge *all_edges, Edge edge, Side side) {
    for (size_t i = 0; i < side.edge_ct; i++) {
        Edge comp = all_edges[side.edge_indicies[i]];
        if(comp.loc == edge.loc && comp.dir == edge.dir) return true;
    }
    return false;
}

Side define_side(Edge *edges, int index, int edge_ct){
    Side ret_side;
    size_t prev_ct = 1;
    ret_side.edge_ct = 1;
    ret_side.edge_indicies[0] = index;
REPEAT:
    prev_ct = ret_side.edge_ct;
    //every edge in side
    for (size_t outer_i = 0; outer_i < ret_side.edge_ct; outer_i++){
        Edge side_member = edges[ret_side.edge_indicies[outer_i]];
        //compare with every other edge
        for (size_t inner_i = 0; inner_i < edge_ct; inner_i++){
            Edge cmp_edge = edges[inner_i];
            //skip if edge already known to be in side
            if((edge_is_in_side(edges, cmp_edge, ret_side))) continue;
            //if edge in question is adjacent to edge in side, append and restart
            if(edges_are_adjacent(cmp_edge, side_member)){
                //append found edge
                ret_side.edge_indicies[ret_side.edge_ct] = inner_i;
                ret_side.edge_ct++;
                goto DOUBLE_BREAK;
            }
        }
    }
DOUBLE_BREAK:
    //repeat until sides edge_ct is no longer increasing
    if(ret_side.edge_ct != prev_ct) goto REPEAT;

    return ret_side;
}

unsigned count_sides(Region *cur, char *puzzle) {
    size_t edge_ct = 0;
    size_t side_ct = 0;
    Edge edges[MAX_EDGES];
    Side sides[MAX_EDGES];
    // define edges
    for (size_t i = 0; i < matrix_sz; i++) {
        if (cur->indicies[i]) {
            char value = puzzle[i];
            for (int dir = 0; dir < DIR_COUNT; dir++) {
                int adj_index = get_adj_index(i, dir, 1);
                if (value == puzzle[adj_index]) continue;  // not an edge
                Edge edge = {.dir = dir, .loc = i};
                edges[edge_ct] = edge;
                edge_ct++;
            }
        }
    }
    //define sides
    for (size_t i = 0; i < edge_ct; i++) {
        bool new_side = true;
        for (size_t j = 0; j < side_ct; j++) {
            if (edge_is_in_side(edges, edges[i], sides[j])) {
                new_side = false;
                break;
            }
        }
        if (new_side) {
            sides[side_ct] = define_side(edges, i, edge_ct);
            side_ct++;
        }
    }
    return side_ct;
}

unsigned price(Region *cur, char *puzzle) {
    unsigned area = 0;
    for (size_t i = 0; i < matrix_sz; i++) {
        if (cur->indicies[i]) area++;
    }
#ifdef PART_TWO
    return area * count_sides(cur, puzzle);
#else
    return area * get_perimeter(cur, puzzle);
#endif
}

int main(int argc, char const *argv[]) {
    char *puzzle = parse_puzzle();
    unsigned result = 0;
    Region *back = NULL;
    Region *cur = NULL;
    bool *fenced = malloc(sizeof(bool) * matrix_sz);
    check_malloc(fenced);
    memset(fenced, false, sizeof(bool) * matrix_sz);
    // make a list of all the regions and their members
    for (size_t i = 0; i < matrix_sz; i++) {
        if (fenced[i]) continue;
        back = append_region(puzzle, i, back);
        for (size_t i = 0; i < matrix_sz; i++) {
            if (back->indicies[i]) fenced[i] = true;
        }
    }
    cur = back;
    // price each region
    while (cur != NULL) {
        unsigned priced = price(cur, puzzle);
        result += priced;
        cur = cur->prev;
    }
    printf("Result is %u\n", result);

    free_regions(back);
    free(puzzle);
    return 0;
}
