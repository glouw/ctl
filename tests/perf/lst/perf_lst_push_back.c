#include "../../test.h"

#define P
#define T int
#include <lst.h>

#include <time.h>

int main(void)
{
    puts(__FILE__);
    srand(time(NULL));
    for(int run = 0; run < TEST_PERF_RUNS; run++)
    {
        lst_int c = lst_int_init();
        int elems = TEST_PERF_CHUNKS * run;
        int t0 = TEST_TIME();
        for(int elem = 0; elem < elems; elem++)
            lst_int_push_back(&c, rand());
        int t1 = TEST_TIME();
        printf("%10d %10d\n", elems, t1 - t0);
        lst_int_free(&c);
    }
}
