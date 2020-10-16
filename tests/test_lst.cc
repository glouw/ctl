#include "test.h"

#include "digi.cc"

#define CTL_T digi
#include <lst.h>

#include <list>

static void
test_equal(lst_digi* a, std::list<DIGI>& b)
{
    assert(lst_digi_size(a) == b.size());
    assert(lst_digi_empty(a) == b.empty());
    assert(*lst_digi_front(a)->value == *b.front().value);
    assert(*lst_digi_back(a)->value == *b.back().value);
    std::list<DIGI>::iterator iter = b.begin();
    lst_digi_it it = lst_digi_it_each(a);
    CTL_FOR(it, {
        assert(*it.ref->value == *iter->value);
        iter++;
    });
}

int
main(void)
{
#if TEST_USE_SRAND == 1
    srand(time(NULL));
#endif
    const size_t iters = rand() % TEST_MAX_ITERS;
    for(size_t i = 0; i < iters; i++)
    {
        size_t size = rand() % 16;
        if(size == 0)
            size = 1;
        lst_digi a = lst_digi_init();
        std::list<DIGI> b;
        for(size_t i = 0; i < size; i++)
        {
            int value = rand() % INT_MAX;
            lst_digi_push_back(&a, digi_init(value));
            b.push_back(DIGI{value});
        }
#define LIST          \
        X(PUSH_BACK)  \
        X(PUSH_FRONT) \
        X(TOTAL)
#define X(name) name,
        enum { LIST };
#undef X
        size_t which = rand() % TOTAL;
#ifdef VERBOSE
#define X(name) #name,
        const char* names[] = { LIST };
#undef X
        printf("-> %s\n", names[which]);
#endif
        switch(which)
        {
            case PUSH_FRONT:
            {
                int value = rand() % INT_MAX;
                lst_digi_push_front(&a, digi_init(value));
                b.push_front(DIGI{value});
                break;
            }
            case PUSH_BACK:
            {
                int value = rand() % INT_MAX;
                lst_digi_push_back(&a, digi_init(value));
                b.push_back(DIGI{value});
                break;
            }
        }
        test_equal(&a, b);
        lst_digi_free(&a);
    }
    TEST_PASS(__FILE__);
}
