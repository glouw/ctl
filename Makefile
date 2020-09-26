CC = gcc

CFLAGS = -isystem. -fsanitize=address -g -Wfatal-errors

SRC = test.c

BIN = test

all:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)
	./$(BIN)
	rm $(BIN)
