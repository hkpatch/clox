#include "common.h"
#include "memory.h"

void *reallocate(void *previous, size_t oldSize, size_t newSize){
    if(newSize == 0){
        free(previous);
        return NULL;
    }

    return realloc(previous, newSize);
}

void *alloc(size_t size){
    void *mem = malloc(size);
    if(mem == 0){
        fprintf(stderr, "Memory alloc error.!");
        exit(1);
    }
    return mem;
}