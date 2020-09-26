#pragma once

#define JOIN(A, B) A##B

#define CAT(A, B) JOIN(A, B)

#define TEMPLATE(name) CAT(CAT(name,_), T)

#define IMPL(name) CAT(C, name)
