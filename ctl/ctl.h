#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#define CTL_CAT(a, b) a##b

#define CTL_PASTE(a, b) CTL_CAT(a, b)

#define CTL_TEMP(type, name) CTL_PASTE(CTL_PASTE(name, _), type)

#define CTL_IMPL(container, name) CTL_PASTE(container, CTL_PASTE(_, name))

#define CTL_FOR(it, ...) \
{                        \
    while(!it.done)      \
    {                    \
        {                \
            __VA_ARGS__  \
        }                \
        it.step(&it);    \
    }                    \
} (void) 0

#define CTL_FOREACH(container, variable, iterator, ...)                                   \
{                                                                                         \
    CTL_IMPL(container, it) iterator = CTL_IMPL(CTL_IMPL(container, it), each)(variable); \
    CTL_FOR(it, __VA_ARGS__);                                                             \
} (void) 0

#define CTL_MUST_ALIGN_16(T) (sizeof(T) == sizeof(char))

#define CTL_LEN(a) (sizeof(a) / sizeof(*a))
