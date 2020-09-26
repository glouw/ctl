CC = gcc

CFLAGS = -std=c89 -isystem. -Wall -Wextra -Wpedantic -Wfatal-errors -fsanitize=address -g

BIN = test

all:
	$(CC) $(CFLAGS) test.c -o $(BIN)
	./$(BIN)
	rm $(BIN)

vec:
	$(CC) $(CFLAGS) ctl/vec.h -E
