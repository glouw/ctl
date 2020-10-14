#pragma once

// SHORT STRING OPTIMIZATIONS NOT SUPPORTED.
#define MIN_STR_SIZE     (30)
#define MAX_VALUE   (INT_MAX)
#define MAX_LETTERS      (23)

#ifdef VERBOSE
#define MAX_SIZE  (4096)
#define MAX_ITERS (8096)
#else
#define MAX_SIZE  (1024)
#define MAX_ITERS (1024)
#endif

#define SIGN(a) ((a < 0) ? (-1) : (a > 0) ? (1) : (0))
