CC = gcc -std=c89

CFLAGS = \
	-I. \
	-Wall -Wextra -Wpedantic -Wfatal-errors -Wno-unused-function\
	-fsanitize=address -Og -g

BIN = test

define run
	$(CC) $(CFLAGS) tests/$(1) -o $(BIN); ./$(BIN)
endef

define expand
	$(CC) $(CFLAGS) ctl/$(1).h -E $(2) 
endef

clean: all
	@rm -f $(BIN)

all:
	@echo "compiler: $(CC)"
	$(call run,test_adeque.c)
	$(call run,test_delist.c)
	$(call run,test_array.c)

adeque:
	$(call expand,$@,-DT=int)

delist:
	$(call expand,$@,-DT=int)

array:
	$(call expand,$@,-DT=int -DS=53)
