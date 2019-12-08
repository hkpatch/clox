#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

typedef struct {
    int count;
    int capacity;
    Entry *entries;
} Table;

// 在这里只支持字符串作为key
// 如果想支持其他的Obj的话只需要将Obj线性化
// 用C99提供的柔性数组就行
typedef struct {
    ObjString *key;
    Value value;
} Entry;

void initTable(Table *table);
void freeTable(Table *table);
bool tableSet(Table *table, ObjString *key, Value value);

#endif