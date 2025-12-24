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
    size_t part_one_ct = 0;
    size_t part_two_ct = 0;
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
            if(distance > 99){
                //passing zero each time
                part_two_ct += distance / 100;
                distance = distance % 100;
            }

            //part_two
            if(dir_is_left) distance = distance * -1;
            new_pos = pos + distance; //where we would be not accounting for overflow
            
            //overflow or underflow, tick up part two if we didn't start or end on 0
            if(new_pos > 99){
                new_pos = new_pos - 100;
                if(new_pos != 0 && pos != 0) part_two_ct++;
            } 
            if(new_pos < 0){
                new_pos = new_pos + 100;   
                if(new_pos != 0 && pos != 0) part_two_ct++;
            }
            pos = new_pos;
            if(pos == 0){
                part_one_ct++;
                part_two_ct++;
            }
    }

    printf("Part one password is %zu\n", part_one_ct);
    printf("Part two password is %zu\n", part_two_ct);
    return 0;
}
