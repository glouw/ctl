#include <stdio.h>
#include <assert.h>

#define T int
#include <ctl/adeque.h>

static int
compare_int(const void* a, const void* b)
{
    const int* aa = (int*) a;
    const int* bb = (int*) b;
    return *aa < *bb;
}

static void
test_adeque_int_sort(void)
{
    int count = 42;
    int i;
    adeque_int ints = adeque_int_init(1, NULL);
    for(i = 0; i < count; i++)
        adeque_int_push_back(&ints, rand());
    adeque_int_sort(&ints, compare_int);
    for(i = ints.begin; i < ints.end - 1; i++)
        assert(ints.value[i] > ints.value[i + 1]);
    adeque_int_free(&ints);
}

int
main(void)
{
    test_adeque_int_sort();
    printf("%s: PASSED\n", __FILE__);
    return 0;
}
