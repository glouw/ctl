#include <stdio.h>
#include <assert.h>

#define T int
#include <ctl/vec.h>

static int
compare_int(const void* a, const void* b)
{
    const int* aa = (int*) a;
    const int* bb = (int*) b;
    return *aa < *bb;
}

static void
test_vec_int_sort(void)
{
    int count = 42;
    int i;
    vec_int ints = vec_int_init(1, NULL);
    for(i = 0; i < count; i++)
        vec_int_push_back(&ints, rand());
    vec_int_sort(&ints, compare_int);
    for(i = ints.begin; i < ints.end - 1; i++)
        assert(ints.value[i] > ints.value[i + 1]);
    vec_int_free(&ints);
}

static void
test_vec_int_compile(void)
{
    vec_int ints = vec_int_init(1, NULL);
    vec_int_realloc(&ints, 0);
    vec_int_push_back(&ints, 1);
    vec_int_push_back(&ints, 2);
    vec_int_push_back(&ints, 3);
    vec_int_push_front(&ints, 4);
    vec_int_push_front(&ints, 5);
    vec_int_push_front(&ints, 6);
    vec_int_peak_back(&ints);
    vec_int_peak_front(&ints);
    vec_int_pop_back(&ints);
    vec_int_pop_front(&ints);
    vec_int_del_back(&ints);
    vec_int_del_front(&ints);
    vec_int_swap(&ints, ints.begin, ints.end - 1);
    vec_int_del(&ints, ints.begin);
    vec_int_data(&ints);
    vec_int_size(&ints);
    vec_int_sort(&ints, NULL);
    vec_int_free(&ints);
    vec_int_zero();
}

int
main(void)
{
    test_vec_int_compile();
    test_vec_int_sort();
    printf("%s: PASSED\n", __FILE__);
    return 0;
}
