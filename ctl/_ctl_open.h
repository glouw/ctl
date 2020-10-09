#include <stdlib.h>
#include <stdbool.h>

#define CAT(A, B) A##B

#define PASTE(A, B) CAT(A, B)

#define TEMP(type, name) PASTE(PASTE(name, _), type)

#define IMPL(container, name) PASTE(container, PASTE(_, name))

#define CTL_FOR(iter, ...) while(!iter.done) { __VA_ARGS__ iter.step(&iter); }
