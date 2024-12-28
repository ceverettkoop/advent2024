#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define TOKEN_MAX 64

static int extract_product(const int file_pos, bool *success, FILE *fp){
    //assume provided fp is at pos
    int c = 0;
    char int_token_a[TOKEN_MAX] = {'\0'};
    char int_token_b[TOKEN_MAX] = {'\0'};
    int token_pos = 0;
    //expecting token to be integer,integer)
    while(c != EOF){
        c = fgetc(fp);
    }


}

int main(int argc, char const *argv[]){
    const char path[] = "./input";
    int c = 0;
    const char begin_token[] = "mul(" ;
    size_t begin_token_len = strlen(begin_token);
    int search_index = 0;
    int pos = 0;
    unsigned result = 0;
    
    // open input
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    //identify instances of mul(
    while(c != EOF){
        c = fgetc(fp);
        pos++;
        if(c == begin_token[search_index]){
            search_index++;
            //success, pos now = index of '('
            if(search_index == begin_token_len){
                bool success = false;
                int product = extract_product(pos, &success, fp);
                //fp returned to pos on success or failure
                if(success) result += product;
            }
            search_index = 0;
        }else{
            search_index = 0;
        }
    }

    printf("Sum of all products is %d", result);
    return 0;
}
