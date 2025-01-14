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
#define MAX_EDGES 2048

enum { RIGHT = 0, DOWN = 1, LEFT = 2, UP = 3 };

// globals
const char path[] = "./phonyinput";
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
        int index;
} Edge;

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

// this needs to be completely written to take into account that two colinear sides can be invalid
// because of the blob jogging in btwn
bool same_side(Edge a, Edge b, char *puzzle) {

    return false;
}

unsigned count_sides(Region *cur, char *puzzle) {
    size_t edge_ct = 0;
    Edge edges[MAX_EDGES];
    int edges_considered[MAX_EDGES];
    size_t considered_ct = 0;
    int same_size_reduction = 0;
    // define edges
    for (size_t i = 0; i < matrix_sz; i++) {
        if (cur->indicies[i]) {
            char value = puzzle[i];
            for (int dir = 0; dir < DIR_COUNT; dir++) {
                int adj_index = get_adj_index(i, dir, 1);
                if (value == puzzle[adj_index]) continue;  // not an edge
                Edge edge = {.dir = dir, .index = i};
                edges[edge_ct] = edge;
                edge_ct++;
            }
        }
    }
    // for n edges that are on the same side as each other reduce perimeter by n-1
    for (size_t i = 0; i < edge_ct; i++) {
        bool already_considered = false;
        for (size_t j = 0; j < considered_ct; j++) {
            if (i == edges_considered[j]) {
                already_considered = true;
                break;
            }
        }
        if (already_considered) continue;
        for (size_t j = i + 1; j < edge_ct; j++) {
            Edge a = edges[i];
            Edge b = edges[j];
            if (same_side(a, b, puzzle)) {
                same_size_reduction--;
                edges_considered[considered_ct] = j;
                considered_ct++;
            }
        }
    }
    return edge_ct + same_size_reduction;
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
