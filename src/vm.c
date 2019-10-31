#include "vm.h"
#include "value.h"
#include "debug.h"
#include "scanner.h"

VM vm;

static void resetStack(){
    vm.stackTop = vm.stack;
}

static InterpretResult run();

void initVM(){
    resetStack();
}

void freeVM(){

}

InterpretResult interpret(char *source){
    initScanner(source);
    int line = -1;
    for(;;){
        Token token = scanToken();
        if(token.line != line){
            line = token.line;
            printf("%4d ", line);
        } else{
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if(token.type == TOKEN_EOF) break;
    }
}

static InterpretResult run(){
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do{ \
        double b = pop(); \
        double a = pop(); \
        push(a op b); \
    } while(0)

    for(;;){
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for(Value *slot = vm.stack; slot < vm.stackTop; slot++){
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()){
            case OP_CONSTANT:
            {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_NEGATE:
                push(-pop());
                break;
            case OP_ADD:        BINARY_OP(+); break;
            case OP_SUBTRACT:   BINARY_OP(-); break;
            case OP_MULTIPLY:   BINARY_OP(*); break;
            case OP_DIVIDE:     BINARY_OP(/); break;
            case OP_RETURN:
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

void push(Value value){
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop(){
    vm.stackTop--;
    return *vm.stackTop;
}