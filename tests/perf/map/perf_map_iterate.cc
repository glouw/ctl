#include "../../test.h"

#include <map>
#include <time.h>

int main()
{
    puts(__FILE__);
    srand(time(NULL));
    for(int run = 0; run < TEST_PERF_RUNS; run++)
    {
        std::map<int, int> c;
        int elems = TEST_PERF_CHUNKS * run;
        for(int elem = 0; elem < elems; elem++)
            c.insert(std::pair<int, int>(rand() % elems, rand() % elems));
        int t0 = TEST_TIME();
        volatile int sum = 0;
        for(auto& x : c)
            sum += x.second;
        int t1 = TEST_TIME();
        printf("%10d %10d\n", elems, t1 - t0);
    }
}
