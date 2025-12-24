#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>

#include "../../include/error.h"
#define FREE_BLOCK -1
#define END_OF_MAP -2

#define PART_TWO

// globals
const char path[] = "./input";

static uint64_t generate_checksum(int64_t *disk, size_t block_ct) {
    uint64_t result = 0;
    int64_t id = 0;
    size_t index = 0;
    while (index != block_ct) {
        id = disk[index];
        if(id != FREE_BLOCK) result += id * index;
        index++;
    }
    return result;
}

//part two
static void defragment_disk(int64_t *disk, size_t block_ct) {
    size_t beg_cursor = 0;
    size_t end_cursor = block_ct - 1;
    int64_t id_max = 0;
    // find max id value
    id_max = disk[end_cursor];
    while (disk[end_cursor] == FREE_BLOCK) {
        id_max = disk[end_cursor];
        end_cursor -= 1;
    }
    // try to move each id
    for (size_t i = 0; i < id_max; i++) {
        int64_t target_id = id_max - i;
        size_t target_ct = 0;
        size_t target_end = 0;
        size_t free_block_sz = 0;
        // reset cursors and value
        end_cursor = block_ct - 1;
        beg_cursor = 0;
        // instance of id in question
        while (disk[end_cursor] != target_id) {
            end_cursor -= 1;
        }
        target_end = end_cursor;
        // count instances of target id
        while (disk[end_cursor] == target_id) {
            target_ct++;
            end_cursor -= 1;
            if (end_cursor == 0) break;  // will occur on last instance
        }
        // now look for free space of size n to move to
        while (true) {
            if (disk[beg_cursor] == FREE_BLOCK) {
                free_block_sz++;
            } else
                free_block_sz = 0;
            if(free_block_sz == target_ct) break;
            beg_cursor++;
            if (beg_cursor == (target_end - target_ct + 1)) goto CONTINUE_LOOP;  // we made it back to where we started
        }
        // free the blocks from where we moved
        for (size_t j = 0; j < target_ct; j++) {
            disk[beg_cursor + 1 - target_ct + j] = target_id;
            disk[target_end - j] = FREE_BLOCK;
        }
    CONTINUE_LOOP:
        continue;
    }
}

// part one solution
static void fragment_disk(int64_t *disk, size_t block_ct) {
    size_t end_index = block_ct - 1;
    size_t beg_index = 0;
    size_t free_ct = 0;
    size_t frees_filled = 0;
    for (size_t i = 0; i < block_ct; i++) {
        if (disk[i] == FREE_BLOCK) free_ct++;
    }
    // until every free block has been filled
    while (frees_filled < free_ct) {
        // if block at end is not free
        if (disk[end_index] != FREE_BLOCK) {
            // find first free block from left
            while (disk[beg_index] != FREE_BLOCK) beg_index++;
            // assignment
            disk[beg_index] = disk[end_index];
            disk[end_index] = FREE_BLOCK;
            // reset cursors
            beg_index = 0;
            end_index = block_ct - 1;
            frees_filled++;
        } else {
            // free end block = move end cursor to left
            end_index--;
        }
    }
}

static void print_disk(int64_t *disk, size_t block_ct) {
    for (size_t i = 0; i < block_ct; i++) {
        if (disk[i] == FREE_BLOCK)
            printf(".");
        else
            printf("%lld", disk[i]);
    }
    printf("\n");
}

static void generate_blocks(int64_t *out_disk, int8_t *disk_map) {
    int8_t map_value = 0;
    int64_t cur_id = 0;
    size_t in_pos = 0;
    size_t out_pos = 0;
    // check for end after file or free space instruction
    while (true) {
        // file
        map_value = disk_map[in_pos];
        if (map_value == END_OF_MAP) break;
        for (size_t i = 0; i < map_value; i++) {
            out_disk[out_pos] = cur_id;
            out_pos++;
        }
        cur_id++;  // increment id of file we are storing
        // free space
        in_pos++;
        map_value = disk_map[in_pos];
        if (map_value == END_OF_MAP) break;
        for (size_t i = 0; i < map_value; i++) {
            out_disk[out_pos] = FREE_BLOCK;
            out_pos++;
        }
        // advance to next file
        in_pos++;
    }
}

static size_t read_disk_map(int8_t **disk_map) {
    int c = 0;
    size_t ct = 0;
    size_t pos = 0;
    size_t block_ct = 0;
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file");
    // measure map and allocate;
    fseek(fp, 0, SEEK_END);
    ct = ftell(fp);
    (*disk_map) = malloc(sizeof(int8_t) * ct);
    check_malloc(disk_map);
    fseek(fp, 0, SEEK_SET);
    // read into array
    while (c != EOF) {
        c = fgetc(fp);
        if (isdigit(c)) {
            (*disk_map)[pos] = c - '0';
            block_ct += (*disk_map)[pos];
        } else {
            if (c == '\n') {
                (*disk_map)[pos] = END_OF_MAP;
            } else if (c != EOF)
                fatal_err("invalid input");
        }
        pos++;
    }
    if (pos != ct + 1) fatal_err("error reading input\n");
    fclose(fp);
    return block_ct;
}

int main(int argc, char const *argv[]) {
    int8_t *disk_map = NULL;
    int64_t *disk = NULL;
    size_t block_ct = 0;
    uint64_t result = 0;

    block_ct = read_disk_map(&disk_map);
    disk = malloc(sizeof(int64_t) * block_ct);
    check_malloc(disk);
    generate_blocks(disk, disk_map);
    //printf("Before (de)fragmentation\n");
    //print_disk(disk, block_ct);
#ifdef PART_TWO
    defragment_disk(disk, block_ct);
#else
    fragment_disk(disk, block_ct);
#endif
    //printf("After (de)fragmentation\n");
    //print_disk(disk, block_ct);
    result = generate_checksum(disk, block_ct);
    printf("Result is %llu\n", result);
    free(disk);
    free(disk_map);
    return 0;
}
