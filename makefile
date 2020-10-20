CC = gcc -std=c99
CXX = g++ -std=c++17

LONG = 0
SANITIZE = 1
SRAND = 1

O0 = 0
O1 = 0
O2 = 0
O3 = 0
Og = 0
Ofast = 0

CFLAGS  = -Ictl
CFLAGS += -Wall -Wextra -Wpedantic -Wfatal-errors -Wshadow
CFLAGS += -march=native
CFLAGS += -g

ifeq (1, $(LONG))
CFLAGS += -Werror
CFLAGS += -DLONG
endif

ifeq (1, $(SANITIZE))
CFLAGS += -fsanitize=address -fsanitize=undefined
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

ifeq (1, $(SRAND))
CFLAGS += -DSRAND
endif

define expand
	@$(CC) $(CFLAGS) ctl/$(1).h -E $(2) | clang-format -style=webkit
endef

BINS = tc99_cc tc99_cxx tlst tstr tvec tvecap

TESTS = tests

test: $(BINS)
	$(foreach bin,$(BINS),./$(bin) &&) exit 0
	@rm -f $(BINS)
	@$(CC) --version
	@$(CXX) --version

tc99_cc: ALWAYS
	$(CC) $(CFLAGS) $(TESTS)/test_c99.c -o $@

tc99_cxx: ALWAYS
	$(CXX) $(CFLAGS) $(TESTS)/test_c99.c -o $@

tlst: ALWAYS
	$(CXX) $(CFLAGS) $(TESTS)/test_lst.cc -o $@

tstr: ALWAYS
	$(CXX) $(CFLAGS) $(TESTS)/test_str.cc -o $@

tvec: ALWAYS
	$(CXX) $(CFLAGS) $(TESTS)/test_vec.cc -o $@

tvecap: ALWAYS
	$(CXX) $(CFLAGS) $(TESTS)/test_vec_capacity.cc -o $@

clean:
	@rm -f $(BINS)

# EXPANSIONS.
str:
	$(call expand,$@)

lst:
	$(call expand,$@,-DCTL_T=int)

vec:
	$(call expand,$@,-DCTL_T=int)

ALWAYS:
