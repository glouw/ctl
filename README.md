# THE C TEMPLATE LIBRARY (CTL)

CTL is a collection of header only files for ISO C89/C90/C99/C11
inspired by the C++ Standard Template Library (STL).

## Usage

```C
#include <stdio.h>

#define T int
#include <ctl/vec.h>

static int
compare_int(const void* a, const void* b)
{
    const int* aa = (ints*) a;
    const int* bb = (ints*) b;
    return *aa < *bb;
}

int main(void)
{
    int i;
    vec_int ints = vec_int_init(4, NULL);
    for(i = 0; i < 100; i++)
        vec_int_push_back(&ints, rand());
    vec_int_sort(&ints, compare_int);
    for(i = 0 ; i < vec_int_size(&ints); i++)
        printf("%d\n", vec_int_data(&ints)[i]);
    vec_int_free(&ints);
}
```

## Running Tests

When invoking make, tests are compiled, run, and cleaned up
using `gcc -std=c89`.

    make

The CTL is also compatible with all versions of C++. Testing the CTL with a
C++ compiler can be done with:

    make CC==g++\ -std=c++11
