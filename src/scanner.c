#include "common.h"
#include "scanner.h"

typedef struct Scanner{
    char *current;
    char *start;
    int line;
} Scanner;

Scanner scanner;

static char peek(){
    return *scanner.current;
}

static char peekNext(){
    return *(scanner.current + 1);
}

static char advance(){
    return *scanner.current++;
}

static bool isAtEnd(){
    return peek() == '\0';
}

static bool match(char c){
    if(isAtEnd()) return false;
    if(peek() != c) return false;

    advance();
    return true;
}

static void skipWhiteSpace(){
    while(1){
        char c = peek();
        switch (c)
        {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if(peekNext() == '/'){
                    while(!isAtEnd() && peek() != '\n') advance();
                }
                break;
            default:
                return;
        }
    }
}

static Token makeToken(TokenType type){
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current-scanner.start);
    token.line = scanner.line;

    return token;
}

static Token errorToken(const char *msg){
    Token token;
    token.type = TOKEN_ERROR;
    token.start = msg;
    token.length = (int)strlen(msg);
    token.line = scanner.line;
}

static Token string(){
    while(peek() != '"' && !isAtEnd()){
        if(peek() == '\n') scanner.line++;
        advance();
    }

    if(isAtEnd()) return errorToken("Unterminated string.");
    return makeToken(TOKEN_STRING);
}

Token scanToken(){
    skipWhiteSpace();
    scanner.start = scanner.current;

    if(isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();
    switch (c)
    {
        case '(': return makeToken(TOKEN_LEFT_BRACE);
        case ')': return makeToken(TOKEN_RIGHT_BRACE);
        case '{': return makeToken(TOKEN_LEFT_PAREN);
        case '}': return makeToken(TOKEN_RIGHT_PAREN);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);

        case '!': 
            return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(match('=' ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL));
        case '>':
            return makeToken(match('=' ? TOKEN_GREATER_EQUAL : TOKEN_GREATER));
        case '<':
            return makeToken(match('=' ? TOKEN_LESS_EQUAL : TOKEN_LESS));
        
        case '"':
            return string();

        default:
            if(isDigit(c)){
                
            }
    }

    
}

void initScanner(char *source){
    scanner.current = source;
    scanner.start = source;
    scanner.line = 1;
}