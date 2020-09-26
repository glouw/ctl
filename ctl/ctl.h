#pragma once

#define CAT(A, B) A##B

#define PASTE(A, B) CAT(A, B)

#define IMPL(name) PASTE(C, PASTE(_, name))

#define TEMPLATE(name) PASTE(PASTE(name, _), T)
