static int
TEMPLATE(compare_asc, T)(const void* a, const void* b)
{
    const T* aa = a;
    const T* bb = b;
    return *aa > *bb;
}

static int
TEMPLATE(compare_dec, T)(const void* a, const void* b)
{
    const T* aa = a;
    const T* bb = b;
    return *aa < *bb;
}

#undef T
