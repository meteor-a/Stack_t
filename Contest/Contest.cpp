#include <iostream>
#include "stack_t.h"

int main() {
    Stack_t stack = {};

    StackConstructor(stack);
    stack.capacity = 100000000;
    for (int ii = 0; ii < 30; ++ii) {
        StackPush(&stack, ii);
    }
    StackPop(&stack);
}
