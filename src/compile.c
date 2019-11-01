#include "compile.h"
#include "scanner.h"

typedef struct Parser{
    Token current;
    Token previous;
    bool hadError;
} Parser;

Parser parser;

static void errorAt(Token *token, const char *msg){
    fprintf(stderr, "[line %d] Error", token->line);

    if(token->type == TOKEN_EOF){

    }
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
    if(scanToken().type == type){
        
    }

    
}

InterpretResult compile(char *source, Chunk *chunk){
    initScanner(source);
    advance();
    //expression();
    consume(TOKEN_EOF, "Expect end of expression.");
}