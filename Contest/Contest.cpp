#include <iostream>
#include "stack_t.h"

int main() {
    Stack_t stack = {};
    StackConstructor(stack);
    
    StackPush(&stack, 5);

    StackTop(&stack);

    StackPop(&stack);

    StackDestructor(&stack);
}
