objects = main.o chunk.o debug.o memory.o value.o vm.o \
	scanner.o compile.o object.o
CC = gcc -Iinclude -std=c11 -g
VPATH = src

all : clox clean

clox : $(objects)
	$(CC) -o clox $(objects)

chunk.o : chunk.c memory.o value.o
	$(CC) -c -o $@ $<

debug.o : debug.c 
	$(CC) -c -o $@ $<

main.o : main.c 
	$(CC) -c -o $@ $<

memory.o : memory.c 
	$(CC) -c -o $@ $<

value.o : value.c 
	$(CC) -c -o $@ $<

vm.o : vm.c 
	$(CC) -c -o $@ $<

scanner.o : scanner.c
	$(CC) -c -o $@ $<

compile.o: compile.c
	$(CC) -c -o $@ $<

object.o: object.c

.PHONY: clean
clean : 
	-rm $(objects)
