#ifndef __CTL_H__
#define __CTL_H__

#include <stdlib.h>
#include <stdbool.h>

#define CTL_CAT(a, b) a##b

#define CTL_PASTE(a, b) CTL_CAT(a, b)

#define CTL_TEMP(type, name) CTL_PASTE(CTL_PASTE(name, _), type)

#define CTL_IMPL(container, name) CTL_PASTE(container, CTL_PASTE(_, name))

#define CTL_FOR(iter, ...) while(!iter.done) { __VA_ARGS__ iter.step(&iter); }

#define CTL_MUST_ALIGN_16(T) (sizeof(T) == sizeof(char))

#endif
