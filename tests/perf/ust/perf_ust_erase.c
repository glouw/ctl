#include "../../test.h"

#define P
#define T int
#include <ust.h>

#include <time.h>

size_t
int_hash(int* x)
{ return abs(*x); }

int
int_equal(int* a, int* b)
{ return *a == *b; }

int main(void)
{
    puts(__FILE__);
    srand(time(NULL));
    for(int run = 0; run < TEST_PERF_RUNS; run++)
    {
        ust_int c = ust_int_init(int_hash, int_equal);
        int elems = TEST_PERF_CHUNKS * run;
        for(int elem = 0; elem < elems; elem++)
            ust_int_insert(&c, rand() % elems);
        int t0 = TEST_TIME();
        for(int elem = 0; elem < elems; elem++)
            ust_int_erase(&c, rand() % elems);
        int t1 = TEST_TIME();
        printf("%10d %10d\n", elems, t1 - t0);
        ust_int_free(&c);
    }
}
