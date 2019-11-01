#ifndef clox_compile_h
#define clox_compile_h

#include "vm.h"

InterpretResult compile(char *source, Chunk *chunk);

#endif