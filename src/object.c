#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type *)allocateObject(sizeof(type), objectType)

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

    return obj;
}

ObjString *copyString(const char *chars, int length){
    char *heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';

    return allocateString(heapChars, length);
}

ObjString *takeString(char *chars, int length){
    return allocateString(chars, length);
}

static void freeObject(Obj *object){
    switch(object->type){
        case OBJ_STRING: {
            ObjString *string = (ObjString *)object;
            // 在原文中用自己的reallocate而不直接用free的原因是为了
            // 方便统计已分配的内存
            free(string->chars);
            free(string);
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