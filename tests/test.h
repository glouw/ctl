#ifndef __TEST_H__
#define __TEST_H__

#define TEST_MIN_STR_SIZE  (30)
#define TEST_ALPHA_LETTERS (23)

#ifdef VERBOSE
#  define TEST_MAX_SIZE  (4096)
#  define TEST_MAX_ITERS (8096)
#else
#  define TEST_MAX_SIZE  (1024)
#  define TEST_MAX_ITERS (1024)
#endif

#define TEST_SIGN(a) ((a < 0) ? (-1) : (a > 0) ? (1) : (0))

#endif
