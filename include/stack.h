#include <stdbool.h>

#ifndef SCC_STACK_HEADER
#define SCC_STACK_HEADER

typedef struct Stack {
    int size;
    void **content;
} Stack;

Stack *stack_new();

void stack_free(Stack *stack);
void stack_push(Stack *stack, void *item);
void *stack_pop(Stack *stack);
void *stack_peek(Stack *stack);
bool stack_empty(Stack *stack);

#endif
