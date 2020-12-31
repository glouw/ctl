CC = gcc -std=c11
CXX = g++ -std=c++17

LONG = 0
SANITIZE = 0
SRAND = 1

O0 = 0
O1 = 0
O2 = 0
O3 = 0
Og = 0
Ofast = 0
Os = 0

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

ifeq (1, $(Os))
CFLAGS += -Os
endif

ifeq (1, $(SRAND))
CFLAGS += -DSRAND
endif

TESTS = \
	tests/func/test_c11 \
	tests/func/test_container_composing \
	tests/func/test_deq \
	tests/func/test_lst \
	tests/func/test_str \
	tests/func/test_pqu \
	tests/func/test_que \
	tests/func/test_set \
	tests/func/test_stk \
	tests/func/test_vec_capacity \
	tests/func/test_vec

EXAMPLES = \
	examples/astar \
	examples/postfix \
	examples/json \
	examples/snow \
	examples/6502

all: $(TESTS)
	$(foreach bin,$(TESTS),./$(bin) &&) exit 0
	@$(CC) --version
	@$(CXX) --version
	@rm -f $(TESTS)

examples: $(EXAMPLES)

clean:
	@rm -f $(TESTS)
	@rm -f $(EXAMPLES)

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
set:
	$(call expand,$@,-DT=int -DP)

examples/astar:                      ALWAYS; $(CC)  $(CFLAGS) $@.c  -o $@
examples/postfix:                    ALWAYS; $(CC)  $(CFLAGS) $@.c  -o $@
examples/json:                       ALWAYS; $(CC)  $(CFLAGS) $@.c  -o $@
examples/snow:                       ALWAYS; $(CC)  $(CFLAGS) $@.c  -o $@
examples/6502:                       ALWAYS; $(CC)  $(CFLAGS) $@.c  -o $@
tests/func/test_c11:                 ALWAYS; $(CC)  $(CFLAGS) $@.c  -o $@
tests/func/test_container_composing: ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_deq:                 ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_lst:                 ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_pqu:                 ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_que:                 ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_set:                 ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_stk:                 ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_str:                 ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_vec_capacity:        ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@
tests/func/test_vec:                 ALWAYS; $(CXX) $(CFLAGS) $@.cc -o $@

define expand
	@$(CC) $(CFLAGS) ctl/$(1).h -E $(2) | clang-format -style=webkit
endef

ALWAYS:
