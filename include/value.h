#ifndef clox_value_h
#define clox_value_h

typedef double Value;

typedef struct ValueArray{
    int count;
    int capacity;
    Value *values;
} ValueArray;

void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void freeValueArray(ValueArray *array);
void printValue(Value value);

#endif