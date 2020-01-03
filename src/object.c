#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type *)allocateObject(sizeof(type), objectType)

static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;

  for (int i = 0; i < length; i++) {                     
    hash ^= key[i];                                      
    hash *= 16777619;                                    
  }                                                      

  return hash;                                           
}  

static Obj *allocateObject(size_t size, ObjType type){
    Obj *obj = NEW(size);
    obj->type = type;

    obj->next = vm.objects;
    vm.objects = obj;

    return obj;
}

static ObjString *allocateString(char *chars, int length){
    // 类似于面向对象的初始化方式，先初始化父类
    ObjString *obj = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    // 初始化子类
    obj->chars = chars;
    obj->length = length;
    tableSet(&vm.strings, obj, NIL_VAL);
    obj->hash = hashString(chars, length);

    return obj;
}

ObjString *copyString(const char *chars, int length){
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, 
        chars, length, hash);
    if (interned != NULL) return interned;

    char *heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';

    return allocateString(heapChars, length);
}

ObjString *takeString(char *chars, int length){
    uint32_t hash = hashString(chars, length);
    ObjString *interned = tableFindString(&vm.strings, 
        chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(chars, char, length + 1);
        return interned;
    }

    return allocateString(chars, length);
}

static void freeObject(Obj *object){
    switch(object->type){
        case OBJ_STRING: {
            ObjString *string = (ObjString *)object;
            // 在原文中用自己的reallocate而不直接用free的原因是为了
            // 方便统计已分配的内存
            FREE_ARRAY(string->chars, char, string->length);
            FREE(string, ObjString);
            //free(string->chars);
            //free(string);
        }
    }
}

void freeObjects(){
    Obj *object = vm.objects;
    while(object){
        Obj *next = object->next;
        freeObject(object);
        object = next;
    }
}

void printObject(Value value){
    switch(OBJ_TYPE(value)){
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}