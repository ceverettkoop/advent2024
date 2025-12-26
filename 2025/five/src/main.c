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

    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    // read ranges
    while (c != EOF) {
        c = getc(fp);
        if (isdigit(c)) {
            last_was_break = false;
            digit_buff[buff_index] = c;
            buff_index++;
        }
        if (c == '-') {
            if(range_ct == RANGE_MAX) fatal_err("Too many ranges\n");
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

    printf("Answer is %zu\n", answer);
    return 0;
}
