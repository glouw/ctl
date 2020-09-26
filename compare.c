int CAT(CAT(compare_, T),_asc)(const void* a, const void* b)
{
    const T* aa = a;
    const T* bb = b;
    return *aa > *bb;
}

int CAT(CAT(compare_, T),_dec)(const void* a, const void* b)
{
    const T* aa = a;
    const T* bb = b;
    return *aa < *bb;
}

#undef T
