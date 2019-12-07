#include <stdio.h>
#include <stdlib.h>

#include "compile.h"
#include "scanner.h"

#define DEBUG_PRINT_CODE

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

/* Compile: 从源代码转换成字节码，SourceCode-->Chunk */

typedef struct Parser{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum{
    PREC_NONE,
    PREC_ASSIGNMENT,    // =
    PREC_OR,            // or
    PREC_AND,           // and
    PREC_EQUALITY,      // == !=
    PREC_COMPARISON,    // < > <= >=
    PREC_TERM,          // + -
    PREC_FACTOR,        // * /
    PREC_UNARY,         // ! -
    PREC_CALL,          // . () []
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Parser parser;
Chunk *compilingChunk;      

static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

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

static void emitReturn(){
    emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value value){
    int index = addConstant(currentChunk(), value);
    if(index > UINT8_MAX){
        error("Too mant constants in one chunk.");
        return 0;
    }

    return (uint8_t)index;
}

static void emitConstant(Value value){
    emitTwoByte(OP_CONSTANT, makeConstant(value));
}


static void endCompiler(){
    emitReturn();

#ifdef DEBUG_PRINT_CODE
    if(!parser.hadError){
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

static void binary(){
    // Remember the operator.
    TokenType operatorType = parser.previous.type;

    // Compile the right operand.
    ParseRule *rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    // Emit the operator instruction.
    switch (operatorType) {
        case TOKEN_PLUS:        emitByte(OP_ADD); break;
        case TOKEN_MINUS:       emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:        emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH:       emitByte(OP_DIVIDE); break;

        case TOKEN_BANG_EQUAL:  emitTwoByte(OP_EQUAL, OP_NOT); break;
        case TOKEN_EQUAL_EQUAL: emitByte(OP_EQUAL); break;
        case TOKEN_GREATER:     emitByte(OP_GREATER); break;
        case TOKEN_GREATER_EQUAL:   emitTwoByte(OP_LESS, OP_NOT); break;
        case TOKEN_LESS:        emitByte(OP_LESS); break;
        case TOKEN_LESS_EQUAL:  emitTwoByte(OP_GREATER, OP_NOT); break;
        default:
            return;
    }
}

static void literal(){
    switch(parser.previous.type){
        case TOKEN_FALSE: emitByte(OP_FALSE); break;
        case TOKEN_TRUE: emitByte(OP_TRUE); break;
        case TOKEN_NIL: emitByte(OP_NIL); break;
        default:
            return;
    }
}

static void unary(){
    TokenType operatorType = parser.previous.type;

    // Compile the operand.
    parsePrecedence(PREC_UNARY);

    switch (operatorType)
    {
    case TOKEN_BANG:
        emitByte(OP_NOT);
        break;

    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;

    default:
        return;
    }
}

static void grouping(){
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number(){
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}

// (3+1)
static void parsePrecedence(Precedence precedence){
    advance();
    // 先处理优先级较高的前缀表达式
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if(prefixRule == NULL){
        error("Expect expression.");
        return;
    }

    prefixRule();

    // 然后如果后面有更高和相等优先级的操作符
    while(precedence <= getRule(parser.current.type)->precedence){
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

void expression(){
    parsePrecedence(PREC_ASSIGNMENT);
}

bool compile(char *source, Chunk *chunk){
    initScanner(source);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    // TODO : expression，连接源代码与字节码的关键点
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");

    /* 因为return语句会弹出执行栈中的一个变量并打印出来，所以这里加上return语句 */
    endCompiler();

    return !parser.hadError;
}


ParseRule rules[] = {                                              
  { grouping, NULL,    PREC_NONE},       // TOKEN_LEFT_PAREN      
  { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_PAREN     
  { NULL,     NULL,    PREC_NONE },       // TOKEN_LEFT_BRACE
  { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_BRACE     
  { NULL,     NULL,    PREC_NONE },       // TOKEN_COMMA           
  { NULL,     NULL,    PREC_NONE },       // TOKEN_DOT             
  { unary,    binary,  PREC_TERM },       // TOKEN_MINUS           
  { NULL,     binary,  PREC_TERM },       // TOKEN_PLUS            
  { NULL,     NULL,    PREC_NONE },       // TOKEN_SEMICOLON       
  { NULL,     binary,  PREC_FACTOR },     // TOKEN_SLASH           
  { NULL,     binary,  PREC_FACTOR },     // TOKEN_STAR            
  { unary,     NULL,   PREC_NONE },       // TOKEN_BANG  !           
  { NULL,     binary,  PREC_EQUALITY },       // TOKEN_BANG_EQUAL      
  { NULL,     NULL,    PREC_NONE },       // TOKEN_EQUAL           
  { NULL,     binary,  PREC_EQUALITY },       // TOKEN_EQUAL_EQUAL     
  { NULL,     binary,  PREC_COMPARISON },       // TOKEN_GREATER         
  { NULL,     binary,  PREC_COMPARISON },       // TOKEN_GREATER_EQUAL   
  { NULL,     binary,  PREC_COMPARISON },       // TOKEN_LESS            
  { NULL,     binary,  PREC_COMPARISON },       // TOKEN_LESS_EQUAL      
  { NULL,     NULL,    PREC_NONE },       // TOKEN_IDENTIFIER      
  { NULL,     NULL,    PREC_NONE },       // TOKEN_STRING          
  { number,   NULL,    PREC_NONE },       // TOKEN_NUMBER          
  { NULL,     NULL,    PREC_NONE },       // TOKEN_AND             
  { NULL,     NULL,    PREC_NONE },       // TOKEN_CLASS           
  { NULL,     NULL,    PREC_NONE },       // TOKEN_ELSE            
  { literal,     NULL,    PREC_NONE },       // TOKEN_FALSE           
  { NULL,     NULL,    PREC_NONE },       // TOKEN_FOR             
  { NULL,     NULL,    PREC_NONE },       // TOKEN_FUN             
  { NULL,     NULL,    PREC_NONE },       // TOKEN_IF              
  { literal,     NULL,    PREC_NONE },       // TOKEN_NIL             
  { NULL,     NULL,    PREC_NONE },       // TOKEN_OR              
  { NULL,     NULL,    PREC_NONE },       // TOKEN_PRINT           
  { NULL,     NULL,    PREC_NONE },       // TOKEN_RETURN          
  { NULL,     NULL,    PREC_NONE },       // TOKEN_SUPER           
  { NULL,     NULL,    PREC_NONE },       // TOKEN_THIS            
  { literal,     NULL,    PREC_NONE },    // TOKEN_TRUE            
  { NULL,     NULL,    PREC_NONE },       // TOKEN_VAR             
  { NULL,     NULL,    PREC_NONE },       // TOKEN_WHILE           
  { NULL,     NULL,    PREC_NONE },       // TOKEN_ERROR           
  { NULL,     NULL,    PREC_NONE },       // TOKEN_EOF             
};  

static ParseRule *getRule(TokenType type){
    return &rules[type];
}