#include "common.h"
#include "chunk.h"

int main(int argc, char *argv[]){
    Chunk chunk;
    initChunk(&chunk);
    for(int i = 0; i < 10; i++){
        writeChunk(&chunk, (uint8_t)i);
    }
    for(int i = 0; i < 10; i++){
        printf("%02x", chunk.code[i]);
    }
    free(&chunk);
    return 0;
}