#include <iostream>
#include "stack_t.h"

int main() {
    Stack_t stack = {};
    StackConstructor(stack);

     for (int ii = 0; ii < 500; ++ii) {
        StackPush(&stack, ii);
    }
     for (int ii = 0; ii < 500; ++ii) {
         StackPop(&stack);
     }
     StackPop(&stack);

    int tt = 0;
}
