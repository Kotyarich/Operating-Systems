#include <values.h>
#include "stack.h"

#define ALLOC_MULTIPLIER 2

Stack *init_stack(size_t size) {
    Stack *stack = malloc(sizeof(Stack));
    if (!stack) {
        return NULL;
    }

    stack->length = 0;
    stack->capacity = size;
    stack->data = calloc(size, sizeof(char *));
    if (!stack->data) {
        free(stack);
        return NULL;
    }

    for (size_t i = 0; i < size; i++) {
        stack->data[i] = malloc(sizeof(char) * PATH_MAX);
        if (!stack->data[i]) {
            free_stack(stack);
            return NULL;
        }
    }

    return stack;
}

int push(Stack *stack, const char *item) {
    strcpy(stack->data[stack->length++], item);

    if (stack->length == stack->capacity) {
        size_t new_capacity = stack->capacity * ALLOC_MULTIPLIER;
        char **new_data = realloc(stack->data, new_capacity * sizeof(char *));
        if (!new_data) {
            free_stack(stack);
            return -1;
        }

        stack->capacity = new_capacity;
        stack->data = new_data;

        for (size_t i = stack->length; i < stack->capacity; i++) {
            stack->data[i] = malloc(PATH_MAX * sizeof(char *));
            if (!stack->data[i]) {
                stack->capacity = i;
                free_stack(stack);
                return -2;
            }
        }
    }

    return 0;
}

const char *pull(Stack *stack) {
    if (!stack->length) {
        return NULL;
    }

    char *str = malloc(strlen(stack->data[--stack->length]) * sizeof(char));
    strcpy(str, stack->data[stack->length]);

    return str;
}

void free_stack(Stack *stack) {
    for (size_t i = 0; i < stack->capacity; i++) {
        free(stack->data[i]);
    }
    free(stack->data);
    free(stack);
}

const char *top(Stack *stack) {
    if (!stack->length) {
        return NULL;
    }

    char *str = malloc(strlen(stack->data[stack->length - 1]) * sizeof(char));
    strcpy(str, stack->data[stack->length - 1]);

    return str;
}
