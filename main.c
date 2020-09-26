#include <stdlib.h>
#include <stdio.h>

#include "misc.c"

#define T int
#include "vec.c"

#define T int
#include "compare.c"

int main(void)
{
    vec_int ints = vec_int_init(4, NULL);
    vec_int_push_front(&ints, 3);
    vec_int_push_front(&ints, 2);
    vec_int_push_front(&ints, 1);
    vec_int_push_back(&ints, 90);
    vec_int_push_back(&ints, 91);
    vec_int_push_back(&ints, 92);
    vec_int_push_back(&ints, 93);
    vec_int_push_back(&ints, 94);
    vec_int_push_back(&ints, 95);
    vec_int_sort(&ints, compare_int_asc);
    for(int i = 0 ; i < vec_int_size(&ints); i++)
        printf("%d\n", vec_int_data(&ints)[i]);
    printf("%p : %d\n", vec_int_data(&ints), vec_int_size(&ints));
    vec_int_free(&ints);
}
