#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../../include/error.h"

#define MAX_PAIRS 2048
#define MAX_UPDATES 2048
#define MAX_DIGITS 8
#define MAX_UPDATE_PAGES 32

const char path[] = "./dummyinput";

typedef struct Pair_tag{
    int before;
    int after;
}Pair;


int main(int argc, char const *argv[]){
    int c = 0;
    int last_c = 0;
    char input_buffer[MAX_DIGITS] = {'\0'};
    int buffer_index = 0;
    int pair_index = 0;
    int update_index = 0;
    int page_index = 0;
    //idg why this doesn't work, it is setting them to 0 not -1 but whatever
    Pair pairs[MAX_PAIRS] = {(Pair){.before = -1, .after = -1}};
    int updates[MAX_UPDATES][MAX_UPDATE_PAGES] = {{-1}};

    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    //parse part one
    while(true){
        last_c = c;
        c = fgetc(fp);
        if(c == '\n' && last_c == '\n') break; //end part one
        if(isdigit(c)){
            input_buffer[buffer_index] = c;
            buffer_index++;
            continue;
        } 
        if(c == '|'){
            pairs[pair_index].before = atoi(input_buffer); 
            buffer_index = 0;
            memset(input_buffer, '\0', MAX_DIGITS);
            continue;
        }
        if(c == '\n'){
            pairs[pair_index].after = atoi(input_buffer); 
            pair_index++;
            buffer_index = 0;
            memset(input_buffer, '\0', MAX_DIGITS);
            continue;
        }
    }

    //parse part two
    while(c != EOF){
        c = fgetc(fp);
        if(isdigit(c)){
            input_buffer[buffer_index] = c;
            buffer_index++;
            continue;
        }
        if(c == ','){
            updates[update_index][page_index] = atoi(input_buffer);
            buffer_index = 0;
            page_index++;
            continue;
        } 
        if(c == '\n'){
            updates[update_index][page_index] = atoi(input_buffer);
            buffer_index = 0;
            page_index = 0;
            update_index++;
            continue;
        } 
    }    
    fclose(fp);

    //cleanup

    return 0;
}
