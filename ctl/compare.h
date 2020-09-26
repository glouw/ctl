#include <ctl/ctl.h>

static int
TEMPLATE(compare_asc)(const void* a, const void* b)
{
    const T* aa = a;
    const T* bb = b;
    return *aa > *bb;
}

static int
TEMPLATE(compare_dec)(const void* a, const void* b)
{
    const T* aa = a;
    const T* bb = b;
    return *aa < *bb;
}

#undef T
