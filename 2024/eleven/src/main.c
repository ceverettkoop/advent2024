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

#define ITERATIONS 75
#define MAX_INPUT_DIGITS 64

// globals
const char path[] = "./input";

typedef struct Stone_tag {
        long value;
        long qty;
        long queued_add;
        struct Stone_tag *prev;
        struct Stone_tag *next;
} Stone;

typedef struct Split_tag {
        long a;
        long b;
} Split;

// credit: https://stackoverflow.com/questions/1068849/how-do-i-determine-the-number-of-digits-of-an-integer-in-c
// overflow check possible does not work, this could be slow
long digit_ct(long n) {
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
    long c = 0;
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
            // NOTE WE ARE ASSUMING EACH INPUT VALUE IS UNIQUE BECAUSE... they are in my input
            cur->qty = 1;
            cur->queued_add = 0;
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

Split split_value(long value) {
    size_t ct = digit_ct(value);
    Split ret;
    if ((ct % 2 != 0)) fatal_err("unreachable\n");
    ret.a = (value / pow(10, (ct / 2)));
    ret.b = (value - pow(10, (ct / 2)) * ret.a);
    return ret;
}

// will make hashmap if this is too inefficient
Stone *find_stone(long value, Stone *ptr) {
    Stone *cur = ptr;
    Stone *back = NULL;
    // work to head
    while (cur != NULL) {
        if (cur->value == value) goto FOUND;
        cur = cur->prev;
    }
    cur = ptr;
    // work to back
    while (cur != NULL) {
        back = cur;
        if (cur->value == value) goto FOUND;
        cur = cur->next;
    }
    // failed to find = we have made it to the back
    // case of new value, add on our new node
    cur = malloc(sizeof(Stone));
    check_malloc(cur);
    cur->value = value;
    cur->qty = 0;
    cur->queued_add = 0;
    cur->prev = back;
    back->next = cur;
    cur->next = NULL;
FOUND:
    return cur;
}

int process(Stone *ptr, Stone *back) {
    if (ptr->qty == 0) return 0;
    if (ptr->value == 0) {
        find_stone(1, ptr)->queued_add += ptr->qty;
        goto END;
    }
    if ((digit_ct(ptr->value) % 2) == 0) {
        Split splitted = split_value(ptr->value);
        find_stone(splitted.a, ptr)->queued_add += ptr->qty;
        find_stone(splitted.b, ptr)->queued_add += ptr->qty;
        goto END;
    }
    find_stone(ptr->value * 2024, ptr)->queued_add += ptr->qty;
END:
    ptr->queued_add -= ptr->qty;
    if (ptr == back) return 1;
    return 0;
}

void process_additions(Stone *head) {
    Stone *cur = head;
    // sum up new points
    while (cur != NULL) {
        cur->qty += cur->queued_add;
        cur->queued_add = 0;
        cur = cur->next;
    }
}

int main(int argc, char const *argv[]) {
    Stone *head = NULL;
    Stone *cur = NULL;
    Stone *back = NULL;
    long result = 0;

    head = parse_input();
    for (size_t i = 0; i < ITERATIONS; i++) {
        // find back
        cur = head;
        while (cur != NULL) {
            back = cur;
            cur = cur->next;
        }
        cur = head;
        while (cur != NULL) {
            if (process(cur, back)) break;
            cur = cur->next;
        }
        process_additions(head);
    }
    cur = head;
    while (cur != NULL) {
        result += cur->qty;
        cur = cur->next;
    }
    free_list(head);
    printf("\nResult is %ld\n", result);
    return 0;
}
