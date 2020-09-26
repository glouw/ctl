CC = gcc

CFLAGS = -std=c90 -isystem. -Wall -Wextra -Wpedantic -Wfatal-errors -fsanitize=address -g

SRC = test.c

BIN = test

all:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)
	./$(BIN)
	rm $(BIN)
