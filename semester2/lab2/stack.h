#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef struct stack {
    char **data;
    size_t length;
    size_t capacity;
} Stack;

Stack *init_stack(size_t size);
int push(Stack *stack, const char *item);
const char *pull(Stack *stack);
const char *top(Stack *stack);
void free_stack(Stack *stack);


#endif //STACK_H
