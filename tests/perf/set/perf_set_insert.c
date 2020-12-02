#include "../../test.h"

#define P
#define T int
#include <set.h>

#include <time.h>

static int compare(int* a, int* b) { return *a == *b ? 0 : *a < *b ? -1 : 1; }

int main(void)
{
    puts(__FILE__);
    srand(time(NULL));
    for(int run = 0; run < TEST_PERF_RUNS; run++)
    {
        set_int c = set_int_init(compare);
        int elems = TEST_PERF_CHUNKS * run;
        int t0 = TEST_TIME();
        for(int elem = 0; elem < elems; elem++)
            set_int_insert(&c, rand() % elems);
        int t1 = TEST_TIME();
        printf("%10d %10d\n", elems, t1 - t0);
        set_int_free(&c);
    }
}
