#include "table.h"
#include "object.h"
#include "memory.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table *table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void freeTable(Table *table) {
    FREE_ARRAY(table->entries, Entry, table->capacity);
    initTable(table);
}

static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
    uint32_t index = key->hash % capacity;
    Entry *tombstones = NULL;
    for(;;) {
        Entry *entry = &entries[index];

        if(entry->key == NULL) {
            // 判断是不是墓碑
            if(IS_NIL(entry->value)) {
                return (tombstones == NULL ? entry : tombstones);
            } else {
                // 是墓碑，记录下第一次出现的位置
                if(tombstones == NULL) tombstones = entry;
            }
        } else if(entry->key == key) {
            return entry;
        }

        // TODO：对于key的比较直接==肯定不行
        // 因为可能有两个相同的字符串，但是在不同的内存地址
        if(entry->key == NULL || entry->key == key) {
            return entry;
        }

        index = (index + 1) % capacity;
    }

    return NULL;
}

static void adjustCapacity(Table *table, int capacity) {
    int i;
    Entry *entries = ALLOCATE(Entry, capacity);

    for(i = 0; i < capacity; i++) {
        entries->key = NULL;
        entries->value = NIL_VAL;
    }

    table->count = 0;
    for(i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];
        if(entry == NULL) continue;

        Entry *newEntry = findEntry(entries, capacity, entry->key);

        newEntry->key = entry->key;
        newEntry->value = entry->value;
        table->count++;
    }

    FREE_ARRAY(table->entries, Entry, table->capacity);
    //free(table->entries);
    table->capacity = capacity;
    table->entries = entries;
}

bool tableSet(Table *table, ObjString *key, Value value) {
    if(table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    Entry *entry = findEntry(table->entries, table->capacity, key);

    bool isNewKey = entry->key == NULL;
    if(isNewKey && IS_NIL(entry->value)) table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool tableGet(Table *table, ObjString *key, Value *value) {
    Entry *entry = findEntry(table->entries, table->capacity, key);

    if(entry == NULL) return false;

    *value = entry->value;
    return true;
}

bool tableDelete(Table *table, ObjString *key) {
    Entry *entry = findEntry(table->entries, table->capacity, key);

    if(entry == NULL) return false;

    // 假被删除，实际还存在，为了保持链接不断，能够Hash继续搜寻
    entry->key = NULL;
    entry->value = BOOL_VAL(true);

    return true;
}

void tableAddAll(Table *from, Table *to) {
    Entry *entry, *newEntry;

    for(int i = 0; i < from->capacity; i++) {
        entry = &from->entries[i];
        if(entry == NULL) continue;

        tableSet(to, entry->key, entry->value);
    }
}

ObjString *tableFindString(Table *table, const char *chars,
        int length, uint32_t hash) {
    if (table->count == 0) return NULL;

    uint32_t index = hash % table->capacity;

    for (;;) {
        Entry *entry = &table->entries[index];

        if (entry->key == NULL) {
            // Stop if we find an empty non-tombstone entry.
            if (IS_NIL(entry->value)) return NULL;
        } else if (entry->key->length == length &&
                entry->key->hash == hash &&
                memcmp(entry->key->chars, chars, length) == 0) {
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}