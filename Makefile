VERBOSE = 0

CC = gcc -std=c99
CXX = g++ -std=c++17

CFLAGS  = -Ictl
CFLAGS += -Wall -Wextra -Wpedantic -Wfatal-errors -Werror
ifeq ($(VERBOSE), 1)
CFLAGS += -fsanitize=address -fsanitize=undefined
CFLAGS += -Og -g
CFLAGS += -DVERBOSE
else
CFLAGS += -O3
endif

BIN = test

define run
	@$1 $(CFLAGS) tests/$(2) -o $(BIN); ./$(BIN) || exit
endef

define expand
	@$(CC) $(CFLAGS) ctl/$(1).h -E $(2)
endef

all: run
	@rm -f $(BIN)

run: version
	$(call run,$(CC),test_c99.c)
	$(call run,$(CXX),test_c99.c)
	$(call run,$(CXX),test_str.cc)
	$(call run,$(CXX),test_vec.cc)

version:
	@$(CC) --version

vec:
	$(call expand,$@,-DT=int)

str:
	$(call expand,$@)

clean:
	@rm -f $(BIN)
