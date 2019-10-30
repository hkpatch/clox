#include "common.h"
#include "memory.h"
#include "value.h"

void initValueArray(ValueArray *array){
    array->count = 0;
    array->capacity = 0;
    array->value = NULL;
}

void writeValueArray(ValueArray *array, Value value){
    if(array->count >= array->capacity){
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(array->capacity);
        array->value = GROW_ARRAY(array->value, Value, oldCapacity, array->capacity);
    }
    array->value[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray *array){
    free(array->value);
    initValueArray(array);
}

void printValue(Value value){
	printf("%g", value);
}