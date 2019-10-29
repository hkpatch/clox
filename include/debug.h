#ifndef clox_debug_h
#define clox_debug_h

#include "chunk.h"

void disassembleChunk(Chunk *chunk, const char *name);
void disassembleInstruction(uint8_t *byte, int offset);

#endif