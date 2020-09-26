CC = gcc -std=c89

CFLAGS = \
	-isystem. \
	-Wall -Wextra -Wpedantic -Wfatal-errors \
	-fsanitize=address -O0 -g

BIN = test

define run
	@ $(CC) $(CFLAGS) tests/$(1) -o $(BIN); ./$(BIN)
endef

define expand
	@ $(CC) $(CFLAGS) ctl/$(1).h -E -DT=int
endef

clean: all
	@rm -f $(BIN)

all:
	@echo "compiler: $(CC)"
	$(call run,test_vec.c)
	$(call run,test_list.c)

vec:
	$(call expand,$@)

list:
	$(call expand,$@)
