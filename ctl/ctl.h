#ifndef __CTL_H__
#define __CTL_H__

#define CAT(A, B) A##B

#define PASTE(A, B) CAT(A, B)

#define IMPL(name) PASTE(C, PASTE(_, name))

#define TEMPLATE(name, TYPE) PASTE(PASTE(name, _), TYPE)

#endif
