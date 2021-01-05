#include "../test.h"
#include "digi.hh"

#define T digi
#include <stk.h>

#include <stack>
#include <algorithm>

#define CHECK(_x, _y) {                                       \
    while(_x.size > 0) {                                      \
        assert(_x.size == _y.size());                         \
        assert(stk_digi_empty(&_x) == _y.empty());            \
        assert(*_y.top().value == *stk_digi_top(&_x)->value); \
        _y.pop();                                             \
        stk_digi_pop(&_x);                                    \
    }                                                         \
}

int
main(void)
{
#ifdef SRAND
    srand(time(NULL));
#endif
    const size_t loops = TEST_RAND(TEST_MAX_LOOPS);
    for(size_t loop = 0; loop < loops; loop++)
    {
        size_t size = TEST_RAND(TEST_MAX_SIZE);
        stk_digi a = stk_digi_init();
        std::stack<DIGI> b;
        for(size_t pushes = 0; pushes < size; pushes++)
        {
            const int value = TEST_RAND(INT_MAX);
            stk_digi_push(&a, digi_init(value));
            b.push(DIGI(value));
        }
        enum
        {
            TEST_EMPTY,
            TEST_SIZE,
            TEST_TOP,
            TEST_PUSH,
            TEST_POP,
            TEST_SWAP,
            TEST_TOTAL,
        };
        int which = TEST_RAND(TEST_TOTAL);
        switch(which)
        {
            case TEST_PUSH:
            {
                const int value = TEST_RAND(INT_MAX);
                b.push(DIGI(value));
                stk_digi_push(&a, digi_init(value));
                CHECK(a, b);
                break;
            }
            case TEST_POP:
            {
                if(a.size > 0)
                {
                    b.pop();
                    stk_digi_pop(&a);
                    CHECK(a, b);
                }
                break;
            }
            case TEST_SWAP:
            {
                stk_digi aa = stk_digi_copy(&a);
                stk_digi aaa = stk_digi_init();
                std::stack<DIGI> bb = b;
                std::stack<DIGI> bbb;
                stk_digi_swap(&aaa, &aa);
                std::swap(bb, bbb);
                CHECK(aaa, bbb);
                stk_digi_free(&aaa);
                CHECK(a, b);
                break;
            }
        }
        CHECK(a, b);
        stk_digi_free(&a);
    }
    TEST_PASS(__FILE__);
}
