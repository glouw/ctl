# THE C TEMPLATE LIBRARY (CTL)

CTL is a header only, type safe, template library for C99.

## Motivation

The purpose of the CTL is to back port useful C++
templates to C99 to make maximum use of C99's fast compile times.

## Usage

main.c:

```C
#include <stdio.h>

#define T double
#include <vec.h>

int main(void)
{
    vec_double celcius = vec_double_construct(NULL, NULL);
    vec_double_push_back(&celcius, 12.8);
    vec_double_push_back(&celcius, 24.1);
    vec_double_push_back(&celcius, 19.4);
    vec_double_it it = vec_double_it_construct(&celcius, 0, it.size, 1);
    CTL_FOR(it, {
        printf("%0.2f\n", *it.value);
    })
    vec_double_destruct(&celcius);
    printf("%s: PASSED\n", __FILE__);
}
```

Compilation then requires adding CTL as a system include:

    gcc -isystem ctl main.c

More complex types, like pointers to built in types or structs,
are fully compatible, but must first be type defined:

```C
typedef char* str;

#define T str
#include <vec.h>

typedef struct
{
    int age;
    double height;
    char* name;
}
person;

#define T person
#include <vec.h>
```

In the event that types use heap allocation, function pointers
for both `copy` and `destruct` can be passed to the container's
construct function to handle heap management. For instance,
given a heaped string type, a vector of strings can be constructed,
looped through, and printed with an iterator:


```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char* str;

void str_destruct(str* s)
{
    free(*s);
}

str str_construct(const char* s)
{
    return strcpy(malloc(strlen(s) + 1), s);
}

str str_copy(str* s)
{
    return str_construct(*s);
}

int str_cmp(const void* a, const void* b)
{
    return strcmp((char*) a, (char*) b);
}

#define T str
#include <vec.h>

int main(void)
{
    vec_str strs = vec_str_construct(str_destruct, str_copy);
    vec_str_push_back(&strs, str_construct("This"));
    vec_str_push_back(&strs, str_construct("Is"));
    vec_str_push_back(&strs, str_construct("A"));
    vec_str_push_back(&strs, str_construct("Test"));
    vec_str copy = vec_str_copy(&strs);
    vec_str_sort(&copy, str_cmp);
    vec_str_it it = vec_str_it_construct(&copy, 0, copy.size, 1);
    CTL_FOR(it, {
        puts(*it.value);
    })
    vec_str_destruct(&strs); // No double free.
    vec_str_destruct(&copy);
}
```

Likewise, containers are type defined and can be templated
from other templates:

```C
#define T int
#include <vec.h>

#define T vec_int
#include <vec.h>
```

This creates a template container named `vec_vec_int`, which is
a vector of vectors of type int, or rather `std::vector<std::vector<int>>`
in C++. When constructing, ensure the copy and destruct callbacks from `vec_int` are passed
to the `vec_vec_int` constructor`, and then simply free `vec_vec_int` at a later date:

```C
int main(void)
{
    vec_vec_int matrix = vec_vec_int_construct(vec_int_destruct, vec_int_copy);
    ...
    vec_vec_int_destruct(&matrix);
}
```

## Running Tests

    make

The first unit tests compiles all templated containers with type
`int` to ensure compilation works with C99.

Each templated container will then be compiled with a C++ compiler
and tested against the C++ standard library (STL) to ensure maximum
compatibility (read: mimicry) with the STL.

## Caveats

A template type and container can only be included once. For instance,
the following will not work:

```C
#define T double
#include <vec.h>

#define T double
#include <vec.h>
```

To lower the number of warnings and errors emitted by CTL templated types,
compile with `-Wfatal-errors` to stop compilation at the first template error.

## Template Expansion Debugging

Templates in `ctl/` can be expanded with `int` and printed to STDOUT for debugging
purposes by invoking make with the container name:

    make vec
