CC = gcc -std=c99
CXX = g++ -std=c++17

CFLAGS = \
	-Ictl \
	-Wall -Wextra -Wpedantic -Wfatal-errors \
	-fsanitize=address -Og -g

BIN = test

define run
	@$1 $(CFLAGS) tests/$(2) -o $(BIN); ./$(BIN)
endef

define expand
	@$(CC) $(CFLAGS) ctl/$(1).h -E $(2)
endef

all: run
	@rm -f $(BIN)

run:
	$(call run,$(CC),test_c99.c)
	$(call run,$(CXX),test_str.cc)
	$(call run,$(CXX),test_vec.cc)

vec:
	$(call expand,$@,-DT=int)

str:
	$(call expand,$@)

clean:
	@rm -f $(BIN)
