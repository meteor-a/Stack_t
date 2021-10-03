#include "stack_t.h"

void StackUnitTest() {
    printf("\nStart unit tests\n");
    StackConstructorAndDestructorTest();
    StackPushTest();
    StackPopTest();
}

void StackConstructorAndDestructorTest() {
    printf("\nStart unit test: StackConstructorAndDestructorTest\n");

    Stack_t stack = {};
    StackConstructor(stack);
    StackDestructor(&stack);

    Stack_t stack2 = {};
    StackConstructor(stack2);
    StackDestructor(&stack2);

    Stack_t stack3 = {};
    StackConstructor(stack3);
    StackDestructor(&stack3);

    Stack_t stack4 = {};
    StackConstructor(stack4);
    StackDestructor(&stack4);

    Stack_t stack5 = {};
    StackConstructor(stack5);
    StackDestructor(&stack5);

    printf("\End unit test: StackConstructorAndDestructorTest\n");
}

void StackPushTest() {
    printf("\nStart unit test: StackPushTest\n");

    Stack_t stack = {};
    StackConstructor(stack);
    size_t max_number = 553;
    for (size_t num_elem = 0; num_elem < max_number; ++num_elem) {
        StackPush(&stack, num_elem);
    }

    for (size_t num_elem = 0; num_elem < max_number; ++num_elem) {
        if (stack.data[num_elem] != num_elem) {
            printf("Unit test StackPushTest failed: Elem num %d", num_elem);
            abort();
        }
    }

    StackDestructor(&stack);

    printf("\End unit test: StackPushTest\n");
}

void StackPopTest() {
    printf("\Start unit test: StackPopTest\n");

    Stack_t stack = {};
    StackConstructor(stack);
    size_t max_number = 553;
    for (size_t num_elem = 0; num_elem <= max_number; ++num_elem) {
        StackPush(&stack, num_elem);
    }

    for (long long num_elem = max_number; num_elem >= 0; --num_elem) {
        if (stack.data[num_elem] != num_elem) {
            printf("Unit test StackPopTest failed: Elem num %d", num_elem);
            abort();
        }
        StackPop(&stack);
    }

    StackDestructor(&stack);

    printf("\End unit test: StackPopTest\n");
}
                                                          