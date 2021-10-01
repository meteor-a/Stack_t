#include <iostream>
#include "stack_t.h"

int main() {
    Stack_t stack = {};
    StackConstructor(stack);

    stack.size = 5;

    StackDestructor(&stack);
}
