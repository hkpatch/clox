#include <stdio.h>
#include <stdlib.h>

#include "compile.h"
#include "scanner.h"

/* Compile: 从源代码转换成字节码，SourceCode-->Chunk */

typedef struct Parser{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

Parser parser;

Chunk *compilingChunk;

static Chunk *currentChunk(){
    /* 当之后函数加入之后，当前的Chunk更复杂，
    所以先用函数包装起来，方便之后修改 */
    return compilingChunk;
}

static void errorAt(Token *token, const char *msg){
    /* 当parser检测到一个错误时，它忽略后面因为当前错误导致的嵌套错误，
       直到遇到语句边界，然后恢复该标志值
    */
    if(parser.panicMode) return;
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if(token->type == TOKEN_EOF){
        fprintf(stderr, " at end");
    } else{
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", msg);
    parser.hadError = true;
}

static void error(const char *msg){
    errorAt(&parser.previous, msg);
}

static void errorAtCurrent(const char *msg){
    errorAt(&parser.current, msg);
}

static void advance(){
    parser.previous = parser.current;

    for(;;){
        parser.current = scanToken();
        if(parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char *msg){
    if(parser.current.type == type){
        advance();
        return;
    }

    errorAtCurrent(msg);
}

/* 往Chunk里面填写字节码 */


static void emitByte(uint8_t byte){
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitTwoByte(uint8_t byte1, uint8_t byte2){
    emitByte(byte1);
    emitByte(byte2);
}

static void endCompiler(){
    emitByte(OP_RETURN);
}

bool compile(char *source, Chunk *chunk){
    initScanner(source);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    //expression();
    consume(TOKEN_EOF, "Expect end of expression.");

    /* 因为return语句会弹出执行栈中的一个变量并打印出来，所以这里加上return语句 */
    endCompiler();

    return !parser.hadError;
}