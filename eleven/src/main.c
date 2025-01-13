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

#define ITERATIONS 75
//making this huge for part
#define MAX_DUPLICATIONS 900000000
#define MAX_INPUT_DIGITS 64

// globals
const char path[] = "./input";
struct Stone_tag **duplication_list = NULL;
size_t duplication_ct = 0;

typedef struct Stone_tag {
        long long value;
        struct Stone_tag *prev;
        struct Stone_tag *next;
} Stone;

// credit: https://stackoverflow.com/questions/1068849/how-do-i-determine-the-number-of-digits-of-an-integer-in-c
// NM this works even though the overflow check may be fake; i had bad input
long long digit_ct(long long n) {
    if (n > DBL_MAX) fatal_err("overflow\n");
    if (n == 0) return 1;
    return floor(log10(llabs(n))) + 1;
}

Stone *parse_input() {
    Stone *head = NULL;
    Stone *prev = NULL;
    Stone *cur = NULL;
    char buf[MAX_INPUT_DIGITS] = {'\0'};
    size_t buf_place = 0;
    long long c = 0;
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
            cur->value = atoll(buf);
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
    long long ct = 0;
    long long old_value = ptr->value;
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
    if ((ct % 2 != 0)) fatal_err("unreachable\n");
    // printf("Pre split is %lld\n", old_value);
    left->value = (old_value / pow(10, (ct / 2)));
    // printf("Post split left is %lld\n", left->value);
    right->value = (old_value - pow(10, (ct / 2)) * left->value);
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
}

int main(int argc, char const *argv[]) {
    Stone *head = NULL;
    Stone *cur = NULL;
    long long result = 0;
    time_t last = time(NULL);
    long elapsed;

    //if I heap allocate this maybe I can have a stupidly huge list
    duplication_list = malloc(sizeof(Stone*) * MAX_DUPLICATIONS);
    check_malloc(duplication_list);

    head = parse_input();

    for (int i = 0; i < ITERATIONS; i++) {
        printf("INFO: On iteration number %d\n", i + 1);
        cur = head;
        while (cur != NULL) {
            process(cur);
            cur = cur->next;
        }
        head = process_duplications(head);
        elapsed = time(NULL) - last;
        printf("Prev iteration took %ld seconds\n", elapsed);
        last = time(NULL);
    }

    cur = head;
    //printf("Final values:\n");
    while (cur != NULL) {
        //printf("%lld ", cur->value);
        cur = cur->next;
        result++;
    }

    free_list(head);
    free(duplication_list);
    printf("\nResult is %lld\n", result);
    return 0;
}
