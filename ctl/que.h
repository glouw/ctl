//
// Queue
//

#ifndef T
#error "Template type T undefined for <que.h>"
#endif

#define push_back   push
#define pop_front   pop
#define at          __AT
#define begin       __BEGIN
#define end         __END
#define push_front  __PUSH_FRONT
#define pop_back    __PUSH_BACK
#define erase       __ERASE
#define insert      __INSERT
#define resize      __RESIZE
#define assign      __ASSIGN
#define clear       __CLEAR
#define ranged_sort __RANGED_SORT
#define sort        __SORT
#define range       __RANGE
#define each        __each
#define step        __STEP
#define remove_if   __REMOVE_IF

#define deq que
#include <deq.h>
#undef deq

#undef push_back
#undef pop_front
#undef at
#undef begin
#undef end
#undef push_front
#undef pop_back
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
