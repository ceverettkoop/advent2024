#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

#include "../../include/error.h"

#define ITERATIONS 25
#define MAX_DUPLICATIONS 2048

// globals
const char path[] = "./input";
struct Stone_tag *duplication_list[MAX_DUPLICATIONS];
size_t duplication_ct = 0;

typedef struct Stone_tag {
        unsigned value;
        struct Stone_tag *prev;
        struct Stone_tag *next;
} Stone;

void duplicate(Stone *ptr){
    unsigned ct = 0;
    unsigned old_value = ptr->value;
    Stone *left = ptr;
    Stone *right = malloc(sizeof(Stone));
    check_malloc(right);
    left->next->prev = right;
    right->next = left->next;
    left->next = right;
    right->prev = left;
    
    ct = digit_ct(ptr->value);
    if((ct % 2 != 0)) fatal_err("unreachable\n");
    left->value = (old_value )


}

Stone *process_duplications(){
    Stone *cur = NULL;
    for (size_t i = 0; i < duplication_ct; i++){
        duplicate(duplication_list[i]);
    }
    cur = duplication_list[0];
    while(cur->prev != NULL){
        cur = cur->prev;
    }
    duplication_ct = 0;
    return cur; //returns the head of the list
}

void queue_duplication(Stone *ptr){
    duplication_list[duplication_ct] = ptr;
    duplication_ct++;
}

// credit: https://stackoverflow.com/questions/1068849/how-do-i-determine-the-number-of-digits-of-an-integer-in-c
unsigned digit_ct(unsigned n) {
    if (n == 0) return 1;
    return floor(log10(abs(n))) + 1;
}

void process(Stone *ptr) {
    if (ptr->value == 0) {
        ptr->value = 1;
        return;
    }
    if ((digit_ct(ptr->value) % 2) == 0) {
        queue_duplication(ptr);
        return;
    }
    ptr->value = ptr->value * 2024;
}

int main(int argc, char const *argv[]) {
    Stone *head = NULL;
    Stone *cur = NULL;

    head = parse_input();
    
    for (size_t i = 0; i < ITERATIONS; i++){
        cur = head;
        while(cur->next != NULL){
            process(cur);
        }
        head = process_duplications();
    }

    free_list(head);
    return 0;
}
