#ifndef STACK_DOUBLE_H
#define STACK_DOUBLE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

const int sz_double = 1000;

typedef struct stack_double {
    double *arr;
    int n;
} stack_double;

void create_double(stack_double* st) {
    st->arr = (double *)malloc(sz_double * sizeof(double));
    st->n = 0;
}

bool push_double(stack_double *st, double value) {
    if (st->n == sz_double) return false;
    st->arr[st->n] = value;
    st->n++;
    return true;
}

double pop_double(stack_double* st) {
    if (st->n == 0) return -1.0;  
    double ret = st->arr[st->n - 1];
    st->n--;
    return ret;
}

bool empty_double(stack_double* st) {
    return st->n == 0;
}

double top_double(stack_double *st) {
    if (empty_double(st)) return -1.0;  
    return st->arr[st->n - 1];
}

#endif 
