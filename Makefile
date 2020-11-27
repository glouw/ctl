CC = gcc -std=c99
CXX = g++ -std=c++17

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

CXXFLAGS = $(CFLAGS)

define expand
	@$(CC) $(CFLAGS) ctl/$(1).h -E $(2) | clang-format -style=webkit
endef

BINS = \
tests/test_c99 \
tests/test_container_composing \
tests/test_deq \
tests/test_lst \
tests/test_map \
tests/test_str \
tests/test_pqu \
tests/test_que \
tests/test_set \
tests/test_stk \
tests/test_vec_capacity \
tests/test_vec

all: $(BINS)
	$(foreach bin,$(BINS),./$(bin) &&) exit 0
	@$(CC) --version
	@$(CXX) --version

TEST_DEPS = tests/test.h
TEST_DEPS_CXX = $(TEST_DEPS) tests/test.h

# CC
tests/test_c99: $(TEST_DEPS)
tests/perf_vector_pop_back: $(TEST_DEPS)
tests/perf_vector_push_back: $(TEST_DEPS)
tests/perf_vector_sort: $(TEST_DEPS)

# CXX
tests/test_container_composing: $(TEST_DEPS_CXX)
tests/test_deq: $(TEST_DEPS_CXX)
tests/test_lst: $(TEST_DEPS_CXX)
tests/test_map: $(TEST_DEPS_CXX)
tests/test_pqu: $(TEST_DEPS_CXX)
tests/test_que: $(TEST_DEPS_CXX)
tests/test_set: $(TEST_DEPS_CXX)
tests/test_stk: $(TEST_DEPS_CXX)
tests/test_str: $(TEST_DEPS_CXX)
tests/test_vec_capacity: $(TEST_DEPS_CXX)
tests/test_vec: $(TEST_DEPS_CXX)
tests/perf_vec_pop_back: $(TEST_DEPS_CXX)
tests/perf_vec_push_back: $(TEST_DEPS_CXX)
tests/perf_vec_sort: $(TEST_DEPS_CXX)

# PERFORMANCE.

perf_vec: \
tests/perf_vector_push_back \
tests/perf_vec_push_back \
tests/perf_vector_pop_back \
tests/perf_vec_pop_back \
tests/perf_vector_sort \
tests/perf_vec_sort
	./$(word 1,$^) >  $@.log
	./$(word 2,$^) >> $@.log
	./$(word 3,$^) >> $@.log
	./$(word 4,$^) >> $@.log
	./$(word 5,$^) >> $@.log
	./$(word 6,$^) >> $@.log

perf: perf_vec

clean:
	@rm -f $(BINS)
	@rm -f *.log

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

set:
	$(call expand,$@,-DT=int -DP)
