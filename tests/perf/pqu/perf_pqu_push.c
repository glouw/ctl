#include "../../test.h"

#define P
#define T int
#include <pqu.h>

#include <time.h>

static int compare(int* a, int* b) { return *a < *b; }

int main(void)
{
    puts(__FILE__);
    srand(time(NULL));
    for(int run = 0; run < TEST_PERF_RUNS; run++)
    {
        pqu_int c = pqu_int_init(compare);
        int elems = TEST_PERF_CHUNKS * run;
        int t0 = TEST_TIME();
        for(int elem = 0; elem < elems; elem++)
            pqu_int_push(&c, rand());
        int t1 = TEST_TIME();
        printf("%10d %10d\n", elems, t1 - t0);
        pqu_int_free(&c);
    }
}
