#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define T int
#include <ctl/vec.h>

static int
compare_int(const void* a, const void* b)
{
    const int* aa = a;
    const int* bb = b;
    return *aa < *bb;
}

void
test_vec_int_sort(void)
{
    int i;
    vec_int ints = vec_int_init(1, NULL);
    for(i = 0; i < 20; i++)
        vec_int_push_back(&ints, rand());
    vec_int_sort(&ints, compare_int);
    for(i = 0; i < vec_int_size(&ints) - 1; i++)
    {
        int* data = vec_int_data(&ints);
        assert(data[i] > data[i + 1]);
    }
    vec_int_free(&ints);
}

void
test_vec_int_del(void)
{
    vec_int ints = vec_int_init(1, NULL);
    vec_int_push_front(&ints, 2);
    vec_int_push_front(&ints, 1);
    vec_int_push_front(&ints, 0);
    vec_int_push_back(&ints, 3);
    vec_int_push_back(&ints, 4);
    vec_int_push_back(&ints, 5);
    vec_int_push_back(&ints, 6);
    vec_int_del(&ints, 2);
    vec_int_del(&ints, 4);
    assert(vec_int_data(&ints)[0] == 1);
    assert(vec_int_data(&ints)[1] == 0);
    assert(vec_int_data(&ints)[2] == 3);
    assert(vec_int_data(&ints)[3] == 4);
    assert(vec_int_data(&ints)[4] == 6);
    vec_int_del_back(&ints);
    assert(vec_int_data(&ints)[0] == 1);
    assert(vec_int_data(&ints)[1] == 0);
    assert(vec_int_data(&ints)[2] == 3);
    assert(vec_int_data(&ints)[3] == 4);
    assert(vec_int_size(&ints) == 4);
    vec_int_del_front(&ints);
    assert(vec_int_data(&ints)[0] == 0);
    assert(vec_int_data(&ints)[1] == 3);
    assert(vec_int_data(&ints)[2] == 4);
    assert(vec_int_size(&ints) == 3);
    vec_int_free(&ints);
}

void
test(void)
{
    test_vec_int_sort();
    test_vec_int_del();
    puts("TESTS PASSED");
}

int
main(void)
{
    test();
    return 0;
}
