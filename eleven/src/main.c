#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include "../../include/error.h"

#define ITERATIONS 25
#define MAX_DUPLICATIONS 900000000
#define MAX_INPUT_DIGITS 64

//12 digits gets us there?
#define VALUE_MAX 999999999999

// globals
const char path[] = "./input";
struct Stone_tag **duplication_list = NULL;
size_t duplication_ct = 0;

typedef struct Stone_tag {
        long value;
        struct Stone_tag *prev;
        struct Stone_tag *next;
} Stone;

// credit: https://stackoverflow.com/questions/1068849/how-do-i-determine-the-number-of-digits-of-an-integer-in-c
// this is supposedly faster than cuter solutions
long digit_ct(long n) {
    if(n > 1000000000000000) fatal_err("digit_ct overflow");
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    if (n < 10000000000) return 10;
    if (n < 100000000000) return 11;
    if (n < 1000000000000) return 12;
    if (n < 10000000000000) return 13;
    if (n < 100000000000000) return 14;
    if (n < 1000000000000000) return 15;
    fatal_err("unreachable\n");
}

Stone *parse_input() {
    Stone *head = NULL;
    Stone *prev = NULL;
    Stone *cur = NULL;
    char buf[MAX_INPUT_DIGITS] = {'\0'};
    size_t buf_place = 0;
    int c = 0;
    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");

    while (c != EOF) {
        c = fgetc(fp);
        if (isdigit(c)) {
            buf[buf_place] = c;
            buf_place++;
        } else {
            // create the node inline here
            cur = malloc(sizeof(Stone));
            check_malloc(cur);
            cur->value = atol(buf);
            if (head == NULL) {
                head = cur;
                prev = cur;
                cur->prev = NULL;
                cur->next = NULL;
            } else {
                cur->prev = prev;
                prev->next = cur;
                cur->next = NULL;
                prev = cur;
            }
            // reset
            buf_place = 0;
            memset(buf, '\0', MAX_INPUT_DIGITS);
        }
    }
    fclose(fp);
    return head;
}

void free_list(Stone *head) {
    Stone *cur = head;
    while (cur->next != NULL) {
        cur = cur->next;
        free(cur->prev);
    }
    free(cur);
}

void duplicate(Stone *ptr) {
    long ct = 0;
    long old_value = ptr->value;
    Stone *left = ptr;
    Stone *right = malloc(sizeof(Stone));
    check_malloc(right);
    if (left->next) {
        left->next->prev = right;
    }
    right->next = left->next;
    left->next = right;
    right->prev = left;

    ct = digit_ct(ptr->value);
    // printf("Pre split is %lld\n", old_value);
    double p = pow(10, (ct / 2));
    left->value = (old_value / p);
    // printf("Post split left is %lld\n", left->value);
    right->value = (old_value - (p * left->value));
    // printf("Post split right is %lld\n", right->value);
}

Stone *process_duplications(Stone *head) {
    Stone *cur = NULL;
    // early exit for empty list
    if (duplication_ct == 0) return head;

    for (size_t i = 0; i < duplication_ct; i++) {
        duplicate(duplication_list[i]);
    }
    cur = duplication_list[0];
    while (cur->prev != NULL) {
        cur = cur->prev;
    }
    duplication_ct = 0;
    return cur;  // returns the head of the list
}

void queue_duplication(Stone *ptr) {
    if (duplication_ct == MAX_DUPLICATIONS) fatal_err("duplication overflow\n");
    duplication_list[duplication_ct] = ptr;
    duplication_ct++;
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
    if(ptr->value > VALUE_MAX) fatal_err("value overflow");
}

int main(int argc, char const *argv[]) {
    Stone *head = NULL;
    Stone *cur = NULL;
    long result = 0;
    time_t start = time(NULL);
    time_t last = time(NULL);
    long elapsed;

    //if I heap allocate this maybe I can have a stupidly huge list
    duplication_list = malloc(sizeof(Stone*) * MAX_DUPLICATIONS);
    check_malloc(duplication_list);

    head = parse_input();
    cur = head;
    //ok iterate one number 75 times, then do then next
    while (cur != NULL) {
        printf("INFO parsing tree starting with %ld\n", cur->value);
        //making a new list of one
        Stone *single = malloc(sizeof(Stone));
        check_malloc(single);
        Stone *inner_cur = NULL;
        single->value = cur->value;
        single->prev = NULL;
        single->next = NULL;
        for (int i = 0; i < ITERATIONS; i++){
            printf("INFO on iteration  %d\n", i);
            inner_cur = single;
            while (inner_cur != NULL) {
                process(inner_cur);
                inner_cur = inner_cur->next;
            }
            //reset head
            single = process_duplications(single);
        }
        inner_cur = single;
        //count result of this tree add to total
        while (inner_cur != NULL) {
            inner_cur = inner_cur->next;
            result++;
        }
        free_list(single);
        cur = cur->next;
    }   

    free_list(head);
    free(duplication_list);
    printf("Total seconds elapsed is %ld\n", time(NULL) - start);
    printf("Result is %ld\n", result);
    return 0;
}
