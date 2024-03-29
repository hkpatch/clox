#include <string.h>

#include "common.h"
#include "memory.h"
#include "value.h"
#include "object.h"

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
    FREE_ARRAY(array->values, Value, array->capacity);
    //free(array->values);
    initValueArray(array);
}

void printValue(Value value){
    switch(value.type){
        case VAL_NIL:    printf("nil"); break;
        case VAL_BOOL:   printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
        case VAL_OBJ:    printObject(value); break;
    }
}

bool valuesEqual(Value a, Value b){
    if(a.type != b.type) return false;

    switch(a.type){
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_BOOL:  return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:   return true;
        case VAL_OBJ: {
            // ObjString *aString = AS_STRING(a);
            // ObjString *bString = AS_STRING(b);
            // return aString->length == bString->length &&
            //     !memcmp(aString->chars, bString->chars, aString->length);
            return AS_OBJ(a) == AS_OBJ(b);
        }        
    }
}