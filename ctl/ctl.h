#ifndef __CTL_H__
#define __CTL_H__

#include <stdlib.h>
#include <stdint.h>

#define CAT(a, b) a##b

#define PASTE(a, b) CAT(a, b)

#define JOIN(prefix, name) PASTE(prefix, PASTE(_, name))

#define SWAP(TYPE, a, b) do { TYPE temp = *(a); *(a) = *(b); *(b) = temp; } while(0)

#define iterate(iterator, ...)        \
    do                                \
    {                                 \
        while(!iterator.done)         \
        {                             \
            __VA_ARGS__               \
            iterator.step(&iterator); \
        }                             \
    }                                 \
    while(0)

#define foreach(container, variable, iterator, ...)                               \
    do                                                                            \
    {                                                                             \
        JOIN(container, it) iterator = JOIN(JOIN(container, it), each)(variable); \
        iterate(iterator, __VA_ARGS__);                                           \
    }                                                                             \
    while(0)

#define len(a) (sizeof(a) / sizeof(*(a)))

#endif
