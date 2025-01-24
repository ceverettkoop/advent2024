#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "../../include/error.h"

#define LINE_MAX 1024
#define DIGIT_MAX 16
#define NO_SOLUTION INT32_MAX
#define MAX_SOLUTIONS 10000
#define MAX_PRESSES 100

typedef struct Solution_tag {
        int a_ct;
        int b_ct;
} Solution;

typedef struct Vector2_tag {
        unsigned x;
        unsigned y;
} Vector2;

typedef struct Machine_tag {
        Vector2 btn_a;
        Vector2 btn_b;
        Vector2 prize;
} Machine;

// globals
const char path[] = "./input";

Vector2 parse_vector(char *line) {
    size_t pos = 0;
    size_t num_pos = 0;
    Vector2 ret;
    char num[DIGIT_MAX] = {'\0'};
    while (line[pos] != 'X') {
        pos++;
    }
    // skip the X+
    pos += 2;
    while (line[pos] != ',') {
        num[num_pos] = line[pos];
        num_pos++;
        pos++;
    }
    ret.x = atoi(num);
    num_pos = 0;
    memset(num, '\0', DIGIT_MAX);
    while (line[pos] != 'Y') {
        pos++;
    }
    // skip Y+
    pos += 2;
    while (line[pos] != '\0') {
        num[num_pos] = line[pos];
        num_pos++;
        pos++;
    }
    ret.y = atoi(num);
    return ret;
}

static Machine *parse_puzzle(size_t *machine_ct) {
    char line_buffer[LINE_MAX];
    int c = 0;
    size_t line_pos = 0;
    Machine *ptr = malloc(sizeof(Machine));
    check_malloc(ptr);
    *machine_ct = 0;
    memset(line_buffer, 0, LINE_MAX * sizeof(char));

    FILE *fp = fopen(path, "r");
    if (fp == NULL) fatal_err("failed to open file\n");
    while (c != EOF) {
        c = fgetc(fp);
        if (c != '\n') {
            line_buffer[line_pos] = c;
            line_pos++;
            if (line_pos == LINE_MAX) fatal_err("line overflow");
        } else {
            switch (line_buffer[7]) {
                case 'A':
                    ptr[*machine_ct].btn_a = parse_vector(line_buffer);
                    break;
                case 'B':
                    ptr[*machine_ct].btn_b = parse_vector(line_buffer);
                    break;
                case 'X':
                    ptr[*machine_ct].prize = parse_vector(line_buffer);
                    (*machine_ct)++;
                    ptr = realloc(ptr, sizeof(Machine) * (*machine_ct + 1));
                    check_malloc(ptr);
                    break;
                default:
                    break;
            }
            memset(line_buffer, 0, LINE_MAX * sizeof(char));
            line_pos = 0;
        }
    }
    fclose(fp);
    return ptr;
}

void solve(bool is_x, Solution *solutions, Machine machine, int *ct) {
    unsigned target = is_x ? machine.prize.x : machine.prize.y;
    unsigned a = is_x ? machine.btn_a.x : machine.btn_a.y;
    unsigned b = is_x ? machine.btn_b.x : machine.btn_b.y;
    *ct = 0;

    bool a_higher = a > b;
    unsigned higher = a_higher ? a : b;
    unsigned lower = !a_higher ? a : b;
    unsigned high_presses = target / higher;
    if(high_presses > MAX_PRESSES) high_presses = MAX_PRESSES;
    unsigned low_presses = 0;

    while (*ct < MAX_SOLUTIONS) {
        if(high_presses <= 0 && low_presses <= 0) break;
        Solution *solution = &(solutions[*ct]);
        int *higher_ct = a_higher ? &(solution->a_ct) : &(solution->b_ct);
        int *lower_ct = !a_higher ? &(solution->a_ct) : &(solution->b_ct);
        int result = high_presses * higher + low_presses * lower;
        int diff = target - result;
        if (diff == 0) {
            *higher_ct = high_presses;
            *lower_ct = low_presses;
            (*ct)++;
            if (high_presses != 0) {
                high_presses--;
            } else {
                break;  // necessarily most press case, we done
            }
            // low case
        } else if (diff > 0) {
            // fell short, increase low presses if divisible,
            if (abs(diff) % lower == 0) {
                low_presses++;
                if(low_presses == MAX_PRESSES) break; //
            }else{
                // not increasing high presses bc we will never end that way, we can try lowering them
                high_presses--;
            }  
            // high case
        } else if (diff < 0) {
            // decrease by low presses if difference is divisible by them
            if (abs(diff) % lower == 0) {
                low_presses--;
            } else if (abs(diff) >= higher) {
                high_presses--;
            } else {
                // never will get there
                break;
            }
        } else {
            fatal_err("Unreachable");
        }
    }
    if(*ct == MAX_SOLUTIONS){
        printf("Max solutions reached something probably went wrong\n");
    }
    return;
}

bool solutions_are_same(Solution a, Solution b){
    if(a.a_ct != b.a_ct) return false;
    if(a.b_ct != b.b_ct) return false;
    return true;
}

int least_tokens_to_solve(Machine machine) {
    Solution x_solutions[MAX_SOLUTIONS];
    int x_ct = 0;
    Solution y_solutions[MAX_SOLUTIONS];
    int y_ct = 0;
    int least = NO_SOLUTION;

    solve(true, x_solutions, machine, &x_ct);
    solve(false, y_solutions, machine, &y_ct);

    bool x_fewer = x_ct < y_ct;
    int fewer_ct = x_fewer ? x_ct : y_ct;
    int greater_ct = !x_fewer ? x_ct : y_ct;
    Solution *fewer_solutions = x_fewer ? x_solutions : y_solutions;
    Solution *greater_solutions = !x_fewer ? x_solutions : y_solutions;
    
    for (size_t i = 0; i < fewer_ct; i++){
        Solution outer = fewer_solutions[i];
        for (size_t j = 0; j < greater_ct; j++){
            Solution inner = greater_solutions[j];
            if(solutions_are_same(outer, inner)){
                int result = (inner.a_ct * 3) + inner.b_ct;
                if(result < least) least = result;
            }
        }
    }
    return least;
}

int main(int argc, char const *argv[]) {
    size_t ct = 0;
    long result = 0;
    Machine *machines = parse_puzzle(&ct);

    for (size_t i = 0; i < ct; i++) {
        int tokens = least_tokens_to_solve(machines[i]);
        if (tokens != NO_SOLUTION) result += tokens;
    }

    printf("Result is %ld\n", result);
    free(machines);
    return 0;
}
