# THE C TEMPLATE LIBRARY (CTL)

CTL is a header only, type safe, template library for C99.

## Motivation

The purpose of the CTL is to back port useful C++
templates to C99 to make maximum use of C99's fast compile times.

## Running Tests

    make

The first unit tests compiles all templated containers with type
`int` to ensure compilation works with C99.

Each templated container will then be compiled with a C++ compiler
and tested against the C++ standard library (STL) to ensure maximum
compatibility (read: mimicry) with the STL.

## Usage


```C
#define T double
#include <vec.h>

#define T int
#include <vec.h>

typedef struct
{
    int age;
    char* name;
}
person;

#define T person
#include <vec.h>
```

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

Templates in `ctl/` can be expanded with `int` and printed to `stdout` for debugging
purposes by invoking make with the container name:

    make vec
