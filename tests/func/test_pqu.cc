#include "../test.h"
#include "digi.hh"

#define T digi
#include <pqu.h>

#include <queue>
#include <algorithm>

#define CHECK(_x, _y) {                                       \
    while(_x.size > 0) {                                      \
        assert(_x.size == _y.size());                         \
        assert(pqu_digi_empty(&_x) == _y.empty());            \
        assert(*_y.top().value == *pqu_digi_top(&_x)->value); \
        _y.pop();                                             \
        pqu_digi_pop(&_x);                                    \
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
        pqu_digi a = pqu_digi_init(digi_compare);
        std::priority_queue<DIGI> b;
        for(size_t pushes = 0; pushes < size; pushes++)
        {
            const int value = TEST_RAND(INT_MAX);
            pqu_digi_push(&a, digi_init(value));
            b.push(DIGI(value));
        }
        enum
        {
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
                pqu_digi_push(&a, digi_init(value));
                CHECK(a, b);
                break;
            }
            case TEST_POP:
            {
                if(a.size > 0)
                {
                    b.pop();
                    pqu_digi_pop(&a);
                    CHECK(a, b);
                }
                break;
            }
            case TEST_SWAP:
            {
                pqu_digi aa = pqu_digi_copy(&a);
                pqu_digi aaa = pqu_digi_init(digi_compare);
                std::priority_queue<DIGI> bb = b;
                std::priority_queue<DIGI> bbb;
                pqu_digi_swap(&aaa, &aa);
                std::swap(bb, bbb);
                CHECK(aaa, bbb);
                pqu_digi_free(&aaa);
                CHECK(a, b);
                break;
            }
        }
        CHECK(a, b);
        pqu_digi_free(&a);
    }
    TEST_PASS(__FILE__);
}
