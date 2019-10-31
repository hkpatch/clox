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

bool isDigit(char c){
    return (c >= '0' && c <= '9');
}

bool isAlpha(char c){
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') || 
        (c == '_');
}

static Token number(){
    while(isDigit(peek())){
        advance();
    }

    if(peek() == '.' && isDigit(peekNext())){
        advance();
        while(isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

static bool checkKeyword(int start, int length, 
    const char *rest, TokenType type){
    if((scanner.current - scanner.start) == length &&
        memcmp(scanner.start + start, rest, length) == 0){
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType(){
    switch (scanner.start[0]){
        case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
        //TODO
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier(){
    while(isDigit(peek()) || isAlpha(peek())){
        advance();
    }

    return makeToken(identifierType());
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
                return number();
            } else if(isAlpha(c)){
                return identifier();
            }
    }

    
}

void initScanner(char *source){
    scanner.current = source;
    scanner.start = source;
    scanner.line = 1;
}