#include <iostream>
#include "stack_t.h"

int main() {
    Stack_t stack;
    StackConstructor(stack);

    StackDestructor(&stack);

    StackPush(&stack, 4);
}
