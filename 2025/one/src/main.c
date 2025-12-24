#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"

#define MAX_NUM_LEN 3

int main(int argc, char const *argv[]) {
    const char path[] = "./input";
    size_t zero_ct = 0;
    int pos = 50;
    int new_pos = 0;
    char c = 0;
    char num_str[4] = {'\0'};
    size_t num_pos = 0;
    bool dir_is_left = false;
    int distance = 0;

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    while (c != EOF) {
        c = fgetc(fp);
        if(c == '\n') goto ROTATE;
        if(c == 'L') dir_is_left = true;
        if(c == 'R') dir_is_left = false;
        if(isnumber(c)){
            if(num_pos == 4) fatal_err("Input number too many digits\n");
            num_str[num_pos] = c;
            num_pos++;
        }
        continue;

        ROTATE:
            //get distance and clear number buffer
            distance = atoi(num_str);
            num_pos = 0;
            memset(num_str, '\0', MAX_NUM_LEN + 1);

            //accounting for more than a full rotation
            if(distance > 99) distance = distance % 100;

            if(dir_is_left) distance = distance * -1;
            new_pos = pos + distance; //where we would be not accounting for overflow

            //overflow or underflow
            if(new_pos > 99) new_pos = new_pos - 100;
            if(new_pos < 0) new_pos = new_pos + 100;
            pos = new_pos;
            if(pos == 0) zero_ct++;
    }

    printf("Password is %zu\n", zero_ct);
    return 0;
}
