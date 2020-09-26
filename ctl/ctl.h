#ifndef __CTL_H__
#define __CTL_H__

#include <stdlib.h>

#define CAT(A, B) A##B

#define PASTE(A, B) CAT(A, B)

#define IMPL(container, name) PASTE(container, PASTE(_, name))

#define TEMPLATE(type, name) PASTE(PASTE(name, _), type)

#endif
