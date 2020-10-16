BIN = test

CC = gcc -std=c99
CXX = g++ -std=c++17

VERBOSE = 0
LONG = 0
SANITIZE = 1

O0 = 0
O1 = 0
O2 = 0
O3 = 0
Og = 0
Ofast = 0

CFLAGS  = -Ictl
CFLAGS += -Wall -Wextra -Wpedantic -Wfatal-errors
CFLAGS += -Werror
CFLAGS += -march=native
CFLAGS += -g

ifeq (1, $(SANITIZE))
CFLAGS += -Og -fsanitize=address -fsanitize=undefined
else
CFLAGS += -O3
endif

ifeq (1, $(Og))
CFLAGS += -Og
endif

ifeq (1, $(O0))
CFLAGS += -O0
endif

ifeq (1, $(O1))
CFLAGS += -O1
endif

ifeq (1, $(O2))
CFLAGS += -O2
endif

ifeq (1, $(O3))
CFLAGS += -O3
endif

ifeq (1, $(Ofast))
CFLAGS += -Ofast
endif

ifeq (1, $(VERBOSE))
CFLAGS += -DVERBOSE
endif

ifeq (1, $(LONG))
CFLAGS += -DLONG
endif

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
	$(call run,$(CXX),test_lst.cc)
	$(call run,$(CXX),test_str.cc)
	$(call run,$(CXX),test_vec.cc)

version:
	@$(CC) --version

vec:
	$(call expand,$@,-DCTL_T=int)

str:
	$(call expand,$@)

lst:
	$(call expand,$@,-DCTL_T=int)

clean:
	@rm -f $(BIN)
