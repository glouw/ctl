# THE C TEMPLATE LIBRARY (CTL)

CTL is a collection of header only files for ISO C89/C90/C99/C11
inspired by the C++ Standard Template Library (STL).

## Usage

```C
#define T int
#include <ctl/deque.h>

#define T float
#include <ctl/list.h>

/* etc */
```

## Running Tests

When invoking make, tests are compiled, ran, and cleaned up
using `gcc -std=c89`.

    make

The CTL is also compatible with all versions of C++. Testing the CTL with a
C++ compiler can be done with:

    make CC==g++\ -std=c++11
