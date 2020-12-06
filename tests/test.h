#ifndef __TEST__H__
#define __TEST__H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

#ifdef LONG
#define TEST_MAX_SIZE  (4096)
#define TEST_MAX_LOOPS (8096)
#else
#define TEST_MAX_SIZE  (512)
#define TEST_MAX_LOOPS (512)
#endif

#define TEST_SIGN(a) ((a < 0) ? (-1) : (a > 0) ? (1) : (0))

#define TEST_PASS(f) printf("%s: PASS\n", f)

#define TEST_RAND(max) (((max) == 0) ? 0 : (rand() % (max)))

#define TEST_PERF_RUNS (100)

#define TEST_PERF_CHUNKS (256)

static inline int
TEST_TIME(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return 1000000 * now.tv_sec + now.tv_usec;
}

#endif
