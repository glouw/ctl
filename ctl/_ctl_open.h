// These macros between <-- START, -- END> are not
// for end user use and will be undefined in _ctl_close.h.
//
// -- START

#define CAT(A, B) A##B

#define PASTE(A, B) CAT(A, B)

#define TEMP(type, name) PASTE(PASTE(name, _), type)

#define IMPL(container, name) PASTE(container, PASTE(_, name))

// -- END

#ifndef __CTL__
#define __CTL__

#include <stdlib.h>
#include <stdbool.h>

#define CTL_FOR(iter, ...) while(!iter.done) { __VA_ARGS__ iter.step(&iter); }

#endif
