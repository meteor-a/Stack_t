#include <iostream>
#include "stack_t.h"

int main() {
    Stack_t stack = {};
    StackConstructor(stack);
    StackDestructor(&stack);
    stack.capacity = 2;
    StackPush(&stack, 5);
}
