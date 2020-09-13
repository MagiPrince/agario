#include "stack.h"

//Create and return a stack
stack_t *init_stack() {
    stack_t *stack = malloc(sizeof(stack_t));
    stack->next = NULL;
    data_stack_t value;
    value.eater = -1;
    value.eaten = -1;
    value.is_food = false;
    stack->value = value;
    return stack;
}

//return if the stack is empty
bool is_empty(stack_t *stack){
    return stack->value.eaten == -1;
}

//Return the first element of the stack
data_stack_t pop(stack_t **stack, pthread_mutex_t *lock){
    lock_mutex(lock);
    data_stack_t head = (*stack)->value;
    stack_t *tmp = *stack;
    *stack = (*stack)->next;
    free(tmp);
    unlock_mutex(lock);
    return head;
}

//Push an element of data_stack_t type in the stack
void push(stack_t **stack, data_stack_t data, pthread_mutex_t *lock){
    stack_t *tmp = malloc(sizeof(stack_t));
    if(tmp){
        lock_mutex(lock);
        tmp->next = *stack;
        tmp->value = data;
        
        *stack = tmp;
        unlock_mutex(lock);
    }
}