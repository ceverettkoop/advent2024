#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../../include/error.h"

#define RANGE_MAX 200
#define DIGIT_BUFFER_MAX 20

typedef struct Range_tag {
        size_t min;
        size_t max;
} Range;

// cmp to yield descending order
int cmp_ranges(const void *a, const void *b) {
    size_t min_a = (*(Range *)a).min;
    size_t min_b = (*(Range *)b).min;
    if (min_a > min_b) return -1;
    if (min_a < min_b) return 1;
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
    while (c != EOF) {
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
    // we will need to sort the ranges by min in descending order
    range_list = malloc(sizeof(Range) * range_ct);
    check_malloc(range_list);
    for (size_t i = 0; i < range_ct; i++) {
        range_list[i].min = mins[i];
        range_list[i].max = maxes[i];
    }
    qsort(range_list, range_ct, sizeof(Range), cmp_ranges);
    for (size_t i = 0; i < range_ct; i++) {
        // add range of cur item
        answer += (range_list[i].max - range_list[i].min) + 1;
        printf("Cur range is %zu min and %zu max we are adding %zu to sum\n", range_list[i].min, range_list[i].max,
            ((range_list[i].max - range_list[i].min) + 1));
        // cur min is always lowest yet
        //  overlap of cur range with prev noted ranges will occur when cur max is higher than a prev noted min
        for (int k = i - 1; k > -1; k--) {
            //(cur max is higher than min at index k, we have overlap
            if (range_list[i].max > range_list[k].min) {
                // deduct overlap, overlap is cur max - min at k
                size_t deduct = (range_list[i].max - range_list[k].min) + 1;
                printf("Deducting %zu for overlap with range min %zu and max %zu\n", deduct, range_list[k].min,
                    range_list[k].max);
                answer -= deduct;
            }
        }
    }
    printf("Answer is %zu\n", answer);
    free(range_list);
    return 0;
}
