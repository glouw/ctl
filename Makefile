CC = gcc

CFLAGS = \
	-std=c89 \
	-isystem. \
	-Wall -Wextra -Wpedantic -Wfatal-errors \
	-fsanitize=address -Og -g

BIN = test

define run
	@ $(CC) $(CFLAGS) tests/$(1) -o $(BIN); ./$(BIN)
endef

define expand
	@ $(CC) $(CFLAGS) ctl/$(1).h -E -DT=int
endef

clean: all
	rm -f $(BIN)

all:
	$(call run,test_vec.c)

vec:
	$(call expand,$@)

list:
	$(call expand,$@)
