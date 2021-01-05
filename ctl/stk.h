//
// Stack
//

#ifndef T
#error "Template type T undefined for <stk.h>"
#endif

#define push_back   push
#define pop_back    pop
#define back        top
#define at          __AT
#define front       __FRONT
#define begin       __BEGIN
#define end         __END
#define push_front  __PUSH_FRONT
#define pop_front   __POP_FRONT
#define erase       __ERASE
#define insert      __INSERT
#define resize      __RESIZE
#define assign      __ASSIGN
#define clear       __CLEAR
#define ranged_sort __RANGED_SORT
#define sort        __SORT
#define range       __RANGE
#define each        __EACH
#define step        __STEP
#define remove_if   __REMOVE_IF

#define deq stk
#include <deq.h>
#undef deq

#undef push_back
#undef pop_back
#undef back
#undef at
#undef front
#undef begin
#undef end
#undef push_front
#undef pop_front
#undef erase
#undef insert
#undef resize
#undef assign
#undef clear
#undef ranged_sort
#undef sort
#undef range
#undef each
#undef step
#undef remove_if
