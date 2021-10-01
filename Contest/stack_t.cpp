#include "stack_t.h"

//TypeError StackConstructor_(Stack_t* stack DEBUG_CODE(, LOCATION location_call)) {
TypeError StackConstructor_(Stack_t* stack, LOCATION location_call) {
    if (_txIsBadReadPtr(stack)) {
        StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, location_call));
    }

    if (CheckIsWasAlreadyConstract(stack)) {
        StackAbort(stack, TypeError::_ERROR_REPEAT_CONSTRACT DEBUG_CODE(, location_call));
    }

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION 
    stack->canary_start = CANARY_DEFAULT_STRUCT_START;
    stack->canary_end   = CANARY_DEFAULT_STRUCT_END;
#endif

#if DEBUG_MODE == DEBUG_MODE_ON
    stack->location = location_call;
#endif

    stack->capacity = DEFAULT_CAPACITY;
    stack->size = 0;

    stack->data = nullptr;

    StackDataAllocation(stack);

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION 
    stack->hash_data   = 0;
    stack->hash_struct = 0;

    stack->hash_struct = HashFunc(stack,       (char*)stack + sizeof(Stack_t));
    stack->hash_data   = HashFunc(stack->data, stack->data + stack->capacity);
#endif

    return TypeError::_SUCCESSFUL;
}

bool CheckIsWasAlreadyConstract(Stack_t* stack DEBUG_CODE(, LOCATION location_call)) {
    if (_txIsBadReadPtr(stack)) {
        StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, location_call));
    }

    if (stack->data     == nullptr &&
        stack->size     == 0       &&
        stack->capacity == 0       ) {

#if STACK_LEVEL_PROTECTION == STACK_CANARY_PROTECTION
        if (stack->canary_start == 0 &&
            stack->canary_end == 0) {

            return false;
        }
#elif STACK_LEVEL_PROTECTION == STACK_HASH_PROTECTION
        if (stack->canary_start == 0 &&
            stack->canary_end   == 0 &&
            stack->hash_struct  == 0 &&
            stack->hash_struct  == 0) {

            return false;
        }
#else
        return false;
#endif
    }

    return true;
}

TypeError StackDestructor(Stack_t* stack) {
    ASSERT_OK(stack);

//    DataPoisonElemsInizialize((*stack), (*stack)->data, (*stack)->data + (*stack)->capacity);

#if STACK_LEVEL_PROTECTION < STACK_CANARY_PROTECTION
    free((*stack)->data);
#endif

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
    free((char*)stack->data - sizeof(StackCanaryElem_t));

    stack->canary_start = -1;
    stack->canary_end   = -1;
#endif

    stack->data = (StackElem_t*)Poison::pNullData;

    stack->capacity = -1;
    stack->size     = -1;

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    stack->hash_data   = -1;
    stack->hash_struct = -1;
#endif

    return TypeError::_SUCCESSFUL;
}

TypeError StackPush(Stack_t* stack, StackElem_t val) {
    ASSERT_OK(stack);

    stack->data[stack->size] = val;
    ++stack->size;

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    HashReCalculate(stack);
#endif
    if (stack->size == stack->capacity) {
        StackDataAllocation(stack);
    }

    ASSERT_OK(stack);

    return TypeError::_SUCCESSFUL;
}

TypeError StackPop(Stack_t* stack) {
    ASSERT_OK(stack);

    if (stack->size - 1 < 0) {
        StackAbort(stack, TypeError::_ERROR_POP_ON_EMPTY_STACK DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, stack->location.var_type, "stack" }));
    }

    stack->data[stack->size - 1] = DEFAULT_EMPTY_ELEM_OF_STACK;
    --stack->size;

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    HashReCalculate(stack);
#endif
    if (stack->capacity == stack->size + STACK_RESIZE_MORE_CONST * 2) {
        StackDataAllocation(stack);
    }

    ASSERT_OK(stack);

    return TypeError::_SUCCESSFUL;
}

StackElem_t StackTop(Stack_t* stack) {
    ASSERT_OK(stack);

    if (stack->size - 1 < 0) {
        StackAbort(stack, TypeError::_ERROR_TOP_ON_EMPTY_STACK DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, stack->location.var_type, "stack" }));
    }

    return stack->data[stack->size - 1];
}

TypeError StackDataAllocation(Stack_t* stack) {
    long long capacity_old         = -1;
    bool is_need_hash_recalculate  = false;
    bool is_need_move_pointer_data = false;

    if (stack->size == stack->capacity) {   // UP
        ASSERT_OK(stack);
        capacity_old = stack->capacity;
        stack->capacity += STACK_RESIZE_MORE_CONST;
        is_need_hash_recalculate  = true;
        is_need_move_pointer_data = true;
    } else if (stack->capacity - STACK_RESIZE_MORE_CONST * 2 >= stack->size) {  // DOWN
        ASSERT_OK(stack);
        stack->capacity -= STACK_RESIZE_MORE_CONST;
        is_need_hash_recalculate  = true;
        is_need_move_pointer_data = true;
    } else if (stack->size == 0 && stack->capacity == DEFAULT_CAPACITY && stack->data ==  nullptr) {   // INIZIALIZE
        if (stack == nullptr) {
            StackAbort(stack, TypeError::_ERROR_NULL_OBJ DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, typeid(StackElem_t).name(), "stack" }));
        }
        if (_txIsBadReadPtr(stack)) {
            StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, typeid(StackElem_t).name(), "stack" }));
        }
    }
    else {
        return TypeError::_SUCCESSFUL;
    }

#if STACK_LEVEL_PROTECTION < STACK_CANARY_PROTECTION
    long long new_size = stack->capacity * sizeof(StackElem_t);

    stack->data = (StackElem_t*)realloc(stack->data, new_size);

    if (stack->data == nullptr) {
        StackAbort(stack, TypeError::_ERROR_NULL_POINTER_DATA DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, typeid(StackElem_t).name(), "stack" }));
    }
#endif

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
    long long new_size = stack->capacity * sizeof(StackElem_t) + 2 * sizeof(StackCanaryElem_t);

    if (is_need_move_pointer_data) {
        stack->data = (StackElem_t*)((char*)stack->data - sizeof(StackCanaryElem_t));
    }
    stack->data = (StackElem_t*) ((char*)realloc((char*)stack->data, new_size));

    if (stack->data == nullptr) {
        StackAbort(stack, TypeError::_ERROR_NULL_POINTER_DATA DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, typeid(StackElem_t).name(), "stack" }));
    }

    *((StackCanaryElem_t*) stack->data) = CANARY_DEFAULT_DATA_START;

    stack->data = (StackElem_t*) ((char*)stack->data + sizeof(StackCanaryElem_t));

    *((StackCanaryElem_t*) (stack->data + stack->capacity)) = CANARY_DEFAULT_DATA_END;
#endif

    if (capacity_old != -1) {
        DataPoisonElemsInizialize(stack->data + capacity_old, stack->data + stack->capacity);
    }

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
    if (is_need_hash_recalculate) {
        HashReCalculate(stack);
    }
#endif

    return TypeError::_SUCCESSFUL;
}

void DataPoisonElemsInizialize(StackElem_t* start, StackElem_t* end) {
    if (end - start < 0) {
        StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__,
                           typeid(StackElem_t).name(), "end - start < 0" }));
    }

    size_t lenght = end - start;

    for (size_t num_elem = 0; num_elem < lenght; ++num_elem) {
        if (_txIsBadReadPtr(start + num_elem)) {
            StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, 
                       typeid(StackElem_t).name(), "start + num_elem" }));
        }
        *(start + num_elem) = DEFAULT_EMPTY_ELEM_OF_STACK;
    }
}

TypeError StackTypeOK(Stack_t* stack) {
#if STACK_LEVEL_PROTECTION == STACK_WITHOUT_PROTECTION
    return TypeError::_SUCCESSFUL;
#endif

#if STACK_LEVEL_PROTECTION == STACK_STAND_PROTECTION
    return StackTypeOKStandartProtection(stack);
#endif

#if STACK_LEVEL_PROTECTION == STACK_CANARY_PROTECTION
    return StackTypeOKCanaryProtection(stack);
#endif // STACK_LEVEL_PROTECTION

#if STACK_LEVEL_PROTECTION == STACK_HASH_PROTECTION
    return StackTypeOKHashProtection(stack);
#endif // STACK_LEVEL_PROTECTION
}

#if STACK_LEVEL_PROTECTION >= STACK_STAND_PROTECTION
TypeError StackTypeOKStandartProtection(Stack_t* stack) {
    if (_txIsBadReadPtr(stack)) {
        return TypeError::_ERROR_SEGMENTATION_FAULT;
    }
    if (stack == nullptr) {
        return TypeError::_ERROR_NULL_OBJ;
    }
    if (stack->data == nullptr) {
        return TypeError::_ERROR_NULL_POINTER_DATA;
    }
    if (_txIsBadReadPtr(stack->data)) {
        return TypeError::_ERROR_SEGMENTATION_FAULT;
    }
    if (stack->size < 0) {
        return TypeError::_ERROR_SIZE_MINUS;
    }
    if (stack->capacity < 16) {
        return TypeError::_ERROR_CAPACITY_CANT_BE_LOW_DEFAULT;
    }
    if (stack->capacity < stack->size) {
        return TypeError::_ERROR_SIZE_BIGGER_CAPACITY;
    }

    return TypeError::_SUCCESSFUL;
}
#endif

#if STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
TypeError StackTypeOKCanaryProtection(Stack_t* stack) {
    TypeError err = StackTypeOKStandartProtection(stack);
    if (err != TypeError::_SUCCESSFUL) {
        StackAbort(stack, err DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, typeid(StackElem_t).name(), "stack" }));
    }

    if (stack->canary_start != CANARY_DEFAULT_STRUCT_START) {
        return TypeError::_ERROR_CANARY_STACK_STRUCT_START_DEAD;
    }
    if (stack->canary_end   != CANARY_DEFAULT_STRUCT_END) {
        return TypeError::_ERROR_CANARY_STACK_STRUCT_END_DEAD;
    }
    if (*((StackCanaryElem_t*) ((char*)stack->data - sizeof(StackCanaryElem_t))) != CANARY_DEFAULT_DATA_START) {
        return TypeError::_ERROR_CANARY_STACK_DATA_START_DEAD;
    }
    if (*((StackCanaryElem_t*) (stack->data + stack->capacity))                  != CANARY_DEFAULT_DATA_END) {
        return TypeError::_ERROR_CANARY_STACK_DATA_END_DEAD;
    }

    return TypeError::_SUCCESSFUL;
}
#endif

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
TypeError StackTypeOKHashProtection(Stack_t* stack) {
    TypeError err = StackTypeOKCanaryProtection(stack);
    if (err != TypeError::_SUCCESSFUL) {
        StackAbort(stack, err DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, typeid(StackElem_t).name(), "stack" }));
    }

    long long saved_hash_struct = stack->hash_struct;
    long long saved_hash_data   = stack->hash_data;

    stack->hash_struct = 0;
    stack->hash_data   = 0;

    long long cur_hash_struct = HashFunc(stack, (char*)stack + sizeof(Stack_t));
    long long cur_hash_data   = HashFunc(stack->data, stack->data + stack->capacity);

    stack->hash_struct = saved_hash_struct;
    stack->hash_data   = saved_hash_data;

    if (cur_hash_struct != saved_hash_struct) {
        return TypeError::_ERROR_HASH_STRUCT_CHANGED;
    }
    if (cur_hash_data != saved_hash_data) {
        return TypeError::_ERROR_HASH_DATA_CHANGED;
    }

    for (int num_ind = 0; num_ind < stack->size; ++num_ind) {
        if (stack->data[num_ind] == DEFAULT_EMPTY_ELEM_OF_STACK) {
            return TypeError::_ERROR_POISON_ELEM_STACK_DATA;
        }
    }

    return TypeError::_SUCCESSFUL;
}
#endif

#if STACK_LEVEL_PROTECTION >= STACK_HASH_PROTECTION
TypeError HashReCalculate(Stack_t* stack) {
    if (_txIsBadReadPtr(stack)) {
        StackAbort(stack, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, location_call));
    }

    TypeError err = TypeError::_SUCCESSFUL;

#if STACK_LEVEL_PROTECTION == STACK_STAND_PROTECTION
    err = StackTypeOKStandartProtection(stack);
#elif STACK_LEVEL_PROTECTION >= STACK_CANARY_PROTECTION
    err = StackTypeOKCanaryProtection(stack);
#endif

    if (err != TypeError::_SUCCESSFUL) {
        StackDump(stack, TypeError::_ERROR_INIZIALIZE_DATA DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, stack->location.var_type, "stack" }));
        abort();
    }

    stack->hash_data   = 0;
    stack->hash_struct = 0;

    long long cur_hash_data = HashFunc(stack->data, stack->data + stack->capacity);
    stack->hash_struct      = HashFunc(stack, (char*)stack + sizeof(Stack_t));
    stack->hash_data        = cur_hash_data;

    return TypeError::_SUCCESSFUL;
}

long long HashFunc(void* start_hash, void* end_hash) {
    if (_txIsBadReadPtr(start_hash)) {
        StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, "void*", "start_hash" }));
    }
    if (_txIsBadReadPtr(end_hash)) {
        StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, "void*", "end_hash" }));
    }

    if (!(start_hash < end_hash)) {
        StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, "void*", "start_hash, end_hash" }));
    }

    long long res_hash_func = 0;
    for (size_t num_byte = 0; num_byte < (char*)end_hash - (char*)start_hash; ++num_byte) {
        if (_txIsBadReadPtr((char*)start_hash + num_byte)) {
            StackAbort(nullptr, TypeError::_ERROR_SEGMENTATION_FAULT DEBUG_CODE(, LOCATION{ __FILE__, __FUNCTION__, __LINE__, "void*", "start_hash" }));
        }
        char byte_value = *((char*)start_hash + num_byte);
        char temp_1 = byte_value & 1;
        char temp_2 = byte_value | 7;
        res_hash_func += (temp_1 << temp_2) * byte_value;
    }

    return res_hash_func;
}
#endif

void StackAbort(Stack_t* stack, TypeError err_ DEBUG_CODE(, LOCATION location_call)) {
    StackDump(stack, err_ DEBUG_CODE(, LOCATION location_call));
    abort();
}

void StackDump(Stack_t* stack, TypeError err_ DEBUG_CODE(, LOCATION location_call)) {

}
