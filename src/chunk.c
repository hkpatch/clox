#include <stdlib.h>
#include "chunk.h"

void initChunk(Chunk *chunk){
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
}

void writeChunk(Chunk *chunk, uint8_t byte){
    if(chunk->count >= chunk->capacity){
        growChunk(chunk);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

void growChunk(Chunk *chunk){
    chunk->capacity = (chunk->capacity == 0 ? 8 : chunk->capacity * 2);
    chunk->code = realloc(chunk->code, chunk->capacity * sizeof(uint8_t));
}

void freeChunk(Chunk *chunk){
    free(chunk->code);
    initChunk(chunk);
}