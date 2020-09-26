# THE C TEMPLATE LIBRARY (CTL)

C needed ISO conforming templates, so here we go.

## Usage

CTL is collection of header only files packaged into standard containers, like `vec`,
which is synonmous to the `C++ vector`.

    #include <stdlib.h>
    #include <stdio.h>
    #include <ctl/ctl.h>

    #define T int
    #include <ctl/vec.h>

    #define T int
    #include <ctl/compare.h>

    int main(void)
    {
        vec_int ints = vec_int_init(4, NULL);
        for(int i = 0; i < 100; i++)
            vec_int_push_back(&ints, rand());
        vec_int_sort(&ints, compare_dec_int);
        for(int i = 0 ; i < vec_int_size(&ints); i++)
            printf("%d\n", vec_int_data(&ints)[i]);
        vec_int_free(&ints);
    }

## Running tests

    make
