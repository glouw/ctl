#pragma once

#include <stdlib.h>

#define CTL_CAT(A, B) A##B

#define CTL_PASTE(A, B) CTL_CAT(A, B)

#define CTL_TEMP(type, name) CTL_PASTE(CTL_PASTE(name, _), type)

#define CTL_IMPL(container, name) CTL_PASTE(container, CTL_PASTE(_, name))

#define CTL_FOR(iter, ...) while(!iter.done) { __VA_ARGS__ iter.step(&iter); }
