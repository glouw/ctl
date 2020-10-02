CC = gcc -std=c99
CXX = g++ -std=c++17

CFLAGS = \
	-Ictl \
	-Wall -Wextra -Wpedantic -Wfatal-errors \
	-fsanitize=address -Og -g

BIN = test

define run
	$1 $(CFLAGS) $(2) -o $(BIN); ./$(BIN)
endef

define expand
	$(CC) $(CFLAGS) ctl/$(1).h -E $(2)
endef

clean: all
	@rm -f $(BIN)

all:
	# ENSURE ALL CTL CONTAINERS COMPILE WITH C99
	$(call run,$(CC),test_c99.c)

	# ENSURE ALL CTL CONTAINERS COMPILE WITH THE
	# LATEST C++ COMPILER AND WORK LIKE THE STL
	$(call run,$(CXX),test_vec_conformance.cc)

vec:
	$(call expand,$@,-DT=int)
