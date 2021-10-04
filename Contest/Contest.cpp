#include <iostream>
#include "stack_t.h"

int main() {
    Stack_t stack = {};

    StackConstructor(stack);
    
    for (int ii = 0; ii < 5111111; ++ii) {
        StackConstructor(stack);
    }
}
