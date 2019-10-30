#include "common.h"
#include "memory.h"
#include "value.h"

void initValueArray(ValueArray *array){
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void writeValueArray(ValueArray *array, Value value){
    if(array->count >= array->capacity){
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(array->capacity);
        array->values = GROW_ARRAY(array->values, Value, oldCapacity, array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray *array){
    free(array->values);
    initValueArray(array);
}

void printValue(Value value){
	printf("%g", value);
}