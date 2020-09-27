CC = gcc -std=c89

CFLAGS = \
	-I. \
	-Wall -Wextra -Wpedantic -Wfatal-errors -Wno-unused-function\
	-fsanitize=address -Og -g

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
	$(call run,test_adeque.c)
	$(call run,test_delist.c)

adeque:
	$(call expand,$@)

list:
	$(call expand,$@)
