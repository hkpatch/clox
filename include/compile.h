#ifndef clox_compile_h
#define clox_compile_h

#include "vm.h"

bool compile(char *source, Chunk *chunk);
void expression();

#endif