#include <assert.h>
#include <complex.h>
#include <ctype.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <inttypes.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <tgmath.h>
#include <threads.h>
#include <time.h>
#include <uchar.h>
#include <wchar.h>
#include <wctype.h>

#include "../test.h"

#define P
#define T int
#include <deq.h>

#define P
#define T int
#include <lst.h>

#define P
#define T int
#include <pqu.h>

#define P
#define T int
#include <que.h>

#define P
#define T int
#include <set.h>

#define P
#define T int
#include <stk.h>

#define P
#define T int
#include <ust.h>

#define P
#define T int
#include <vec.h>

#include <str.h>

int
main(void)
{
    TEST_PASS(__FILE__);
}
