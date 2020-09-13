#ifndef _STACK_H_
#define _STACK_H_ 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lib/thread.h"

typedef struct{
    bool is_food;
    int eater;
    int eaten;
} data_stack_t;

typedef struct stack{
    data_stack_t value;
    struct stack *next;
} stack_t;

stack_t *init_stack();
bool is_empty(stack_t *stack);
data_stack_t pop(stack_t **stack, pthread_mutex_t *lock);
void push(stack_t **stack, data_stack_t data, pthread_mutex_t *lock);

#endif