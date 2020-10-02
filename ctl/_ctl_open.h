#include <stdlib.h>

#define CAT(A, B) A##B

#define PASTE(A, B) CAT(A, B)

#define TEMP(type, name) PASTE(PASTE(name, _), type)

#define IMPL(container, name) PASTE(container, PASTE(_, name))

#ifndef __CTL__
#define __CTL__

#define CTL_FOR(iter, ...) while(!iter.done) { __VA_ARGS__ iter.step(&iter); }

#endif
