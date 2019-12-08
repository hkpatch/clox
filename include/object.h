#ifndef clox_object_h
#define clox_object_h

#include <stdbool.h>
#include "value.h"

typedef enum {
    OBJ_STRING,
    OBJ_FUNCTION,
} ObjType;

typedef struct sObj{
    ObjType type;
    struct sObj *next;
} Obj;

typedef struct sObjString {
    Obj obj;
    int length;
    char *chars;
    uint32_t hash;
} ObjString;

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (AS_STRING(value)->chars)

#define IS_STRING(value) isObjType(value, OBJ_STRING)

// 为什么要使用一个静态函数？
// 因为里面使用了两次value，如果是写成宏则value会被计算两次
// 比如传入的value为pop()函数调用，那么就会被调用两次
static bool isObjType(Value value, ObjType type){
    return IS_OBJ(value) && OBJ_TYPE(value) == type;
}

ObjString *copyString(const char *chars, int length);
void printObject(Value value);
ObjString *takeString(char *chars, int length);
void freeObjects();

#endif