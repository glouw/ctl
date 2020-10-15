VERBOSE = 0
O0 = 0
O1 = 0
O2 = 0
O3 = 0
Og = 0
Ofast = 0

CC = gcc -std=c99
CXX = g++ -std=c++17

CFLAGS  = -Ictl
CFLAGS += -Wall -Wextra -Wpedantic -Wfatal-errors -Werror
CFLAGS += -Wstringop-overflow=0
CFLAGS += -Warray-bounds=0
CFLAGS += -fsanitize=address -fsanitize=undefined
CFLAGS += -g
CFLAGS += -march=native

ifeq ($(Og),1)
CFLAGS += -Og
endif

ifeq ($(O0),1)
CFLAGS += -O0
endif

ifeq ($(O1),1)
CFLAGS += -O1
endif

ifeq ($(O2),1)
CFLAGS += -O2
endif

ifeq ($(O3),1)
CFLAGS += -O3
endif

ifeq ($(Ofast),1)
CFLAGS += -Ofast
endif

ifeq ($(VERBOSE),1)
CFLAGS += -DVERBOSE
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
