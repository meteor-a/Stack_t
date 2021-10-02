#include <iostream>
#include "stack_t.h"

int main() {
    Stack_t stack = {};
    StackConstructor(stack);

    stack.capacity = 68;
    //StackDestructor(&stack);

    StackPush(&stack, 4);
}
