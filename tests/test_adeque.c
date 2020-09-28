#include <stdio.h>
#include <assert.h>

#define T int
#include <ctl/adeque.h>

#define T adeque_int
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

static void
test_adeque_adeque_int_free(void)
{
    adeque_adeque_int adeques = adeque_adeque_int_init(8, adeque_int_free);
    int i;
    int j;
    for(i = 0; i < 10; i++)
    {
        adeque_int adeque = adeque_int_init(32, NULL);
        for(j = 0; j < 10; j++)
            adeque_int_push_back(&adeque, i * j);
        adeque_adeque_int_push_back(&adeques, adeque);
    }
    i = 4;
    j = 3;
    {
        adeque_int a = adeques.value[adeques.begin + i];
        int value = a.value[a.begin + j];
        assert(value == i * j);
        adeque_adeque_int_free(&adeques);
    }
}

int
main(void)
{
    test_adeque_int_sort();
    test_adeque_adeque_int_free();
    printf("%s: PASSED\n", __FILE__);
    return 0;
}
