#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"

#define RANGE_MAX 200
#define DIGIT_BUFFER_MAX 20

typedef struct Range_tag {
        size_t min;
        size_t max;
} Range;

// cmp for sorting with lowest first
int cmp_ranges(const void *a, const void *b) {
    size_t max_a = (*(Range *)a).max;
    size_t max_b = (*(Range *)b).max;
    if (max_a > max_b) return 1;
    if (max_a < max_b) return -1;
    return 0;
}

int main(int argc, char const *argv[]) {
    char c = 0;
    const char path[] = "./input";
    size_t answer = 0;
    size_t mins[RANGE_MAX] = {0};
    size_t maxes[RANGE_MAX] = {0};
    size_t range_ct = 0;
    char digit_buff[DIGIT_BUFFER_MAX] = {'\0'};
    size_t buff_index = 0;
    bool last_was_break = false;
    Range *range_list = NULL;

    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // read ranges; could be read direct into structs but didn't write it that way and not going to fix it
    while (c != EOF) {
        c = getc(fp);
        if (isdigit(c)) {
            last_was_break = false;
            digit_buff[buff_index] = c;
            buff_index++;
        }
        if (c == '-') {
            if (range_ct == RANGE_MAX) fatal_err("Too many ranges\n");
            mins[range_ct] = atol(digit_buff);
            buff_index = 0;
            memset(digit_buff, '\0', DIGIT_BUFFER_MAX);
        }
        if (c == '\n') {
            // two line breaks in a row, we are done reading ranges
            if (last_was_break) break;
            last_was_break = true;
            maxes[range_ct] = atol(digit_buff);
            buff_index = 0;
            memset(digit_buff, '\0', DIGIT_BUFFER_MAX);
            range_ct++;
        }
    }

    /*PART ONE
    // ranges should be set now; so we will check ids
    while (c != EOF)
        c = getc(fp);
        if (isdigit(c)) {
            digit_buff[buff_index] = c;
            buff_index++;
        }
        if (c == '\n') {
            //actually checking here
            size_t id = atol(digit_buff);
            for (size_t i = 0; i < range_ct; i++){
                if(id < mins[i]) continue;
                if(id > maxes[i]) continue;
                //ID is in this range, done with this id
                answer++;
                break;
            }
            buff_index = 0;
            memset(digit_buff, '\0', DIGIT_BUFFER_MAX);
        }
    }
    */

    /* PART TWO*/
    size_t abs_min = SIZE_MAX;
    size_t abs_max = 0;

    range_list = malloc(sizeof(Range) * range_ct);
    check_malloc(range_list);
    for (size_t i = 0; i < range_ct; i++) {
        range_list[i].min = mins[i];
        range_list[i].max = maxes[i];
    }

    //sort low to high on max
    qsort(range_list, range_ct, sizeof(Range), cmp_ranges);

    //find abs min
    for (size_t i = 0; i < range_ct; i++) {
        if (abs_min > mins[i]) abs_min = mins[i];
    }
    //find abs max
    for (size_t i = 0; i < range_ct; i++) {
        if (abs_max < maxes[i]) abs_max = maxes[i];
    }

    size_t n = abs_min;
    printf("abs_min is %zu\n", abs_min);
    printf("abs_max is %zu\n", abs_max);
    while (n <= abs_max){
        bool fresh = false;
        for (size_t i = 0; i < range_ct; i++) {
            if (n >= range_list[i].min && n <= range_list[i].max){
                printf("INFO: id %zu to %zu are good\n", range_list[i].min, range_list[i].max);
                answer += (range_list[i].max - range_list[i].min) + 1;
                n = range_list[i].max + 1;
                fresh = true;
                break;
            } 
        }
        if(!fresh){
            printf("ID %zu is bad\n", n);
            n++;
        } 
    }
    printf("Answer is %zu\n", answer);
    free(range_list);
    return 0;
}
