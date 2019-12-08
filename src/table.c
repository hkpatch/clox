#include "table.h"

void initTable(Table *table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void freeTable(Table *table) {

}

bool tableSet(Table *table, ObjString *key, Value value) {
    
}