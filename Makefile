CC = gcc -std=c99
CXX = g++ -std=c++20

LONG = 0
SANITIZE = 0
SRAND = 1
COMPARE_A = 0

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

ifeq (1, $(COMPARE_A))
CFLAGS += -DCOMPARE_A
endif

define expand
	@$(CC) $(CFLAGS) ctl/$(1).h -E $(2) | clang-format -style=webkit
endef

BINS = tc99_cc tc99_cxx tdeq tstk tque tpqu tlst tstr tvec tmap tvecap tccomp astar

all: $(BINS) examples
	$(foreach bin,$(BINS),./$(bin) &&) exit 0
	@$(CC) --version
	@$(CXX) --version

tc99_cc: ALWAYS
	$(CC) $(CFLAGS) tests/test_c99.c -o $@

tc99_cxx: ALWAYS
	$(CXX) $(CFLAGS) tests/test_c99.c -o $@

tdeq: ALWAYS
	$(CXX) $(CFLAGS) tests/test_deq.cc -o $@

tstk: ALWAYS
	$(CXX) $(CFLAGS) tests/test_stk.cc -o $@

tque: ALWAYS
	$(CXX) $(CFLAGS) tests/test_que.cc -o $@

tpqu: ALWAYS
	$(CXX) $(CFLAGS) tests/test_pqu.cc -o $@

tlst: ALWAYS
	$(CXX) $(CFLAGS) tests/test_lst.cc -o $@

tstr: ALWAYS
	$(CXX) $(CFLAGS) tests/test_str.cc -o $@

tvec: ALWAYS
	$(CXX) $(CFLAGS) tests/test_vec.cc -o $@

tmap: ALWAYS
	$(CXX) $(CFLAGS) tests/test_map.cc -o $@

tvecap: ALWAYS
	$(CXX) $(CFLAGS) tests/test_vec_capacity.cc -o $@

tccomp: ALWAYS
	$(CXX) $(CFLAGS) tests/test_container_composing.cc -o $@

astar: ALWAYS
	$(CC) $(CFLAGS) examples/astar.c -o $@

clean:
	@rm -f $(BINS)

# EXPANSIONS.
str:
	$(call expand,$@)

lst:
	$(call expand,$@,-DT=int -DP)

vec:
	$(call expand,$@,-DT=int -DP)

deq:
	$(call expand,$@,-DT=int -DP)

stk:
	$(call expand,$@,-DT=int -DP)

que:
	$(call expand,$@,-DT=int -DP)

pqu:
	$(call expand,$@,-DT=int -DP)

map:
	$(call expand,$@,-DT=int -DU=int -DP)

ALWAYS:
