#ifndef STACK_H
#define STACK_H

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
const int sz=1000;
typedef struct stack{
	char *arr;
	int n;
}stack;


void create(stack* st)
{
	st->arr=(char *)malloc(sz);
	st->n=0;
}

bool push(stack *st,char ch)
{
	if(st->n==sz) return 0;
	st->arr[st->n]=ch;
	st->n++;
	return true;
}

char pop(stack* st)
{
	if(st->n==0) return '#';
	char ret=st->arr[st->n-1];
	st->n--;
	return ret;
}

bool empty(stack* st)
{
	return st->n==0;
}

char top(stack *st)
{
	if(empty(st)) return '#';
	return st->arr[st->n-1];
}


#endif
