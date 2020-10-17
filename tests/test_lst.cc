#include "test.h"

#include "digi.cc"

#define CTL_T digi
#include <lst.h>

#include <list>

static void
test_equal(lst_digi* a, std::list<DIGI>& b)
{
    assert(lst_digi_size(a) == b.size());
    if(lst_digi_size(a) && b.size() > 0)
    {
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
}

int
main(void)
{
#ifdef SRAND
    srand(time(NULL));
#endif
    const size_t iters = rand() % TEST_MAX_ITERS;
    for(size_t i = 0; i < iters; i++)
    {
        size_t size = rand() % 128;
        size_t min = 2;
        if(size < min)
            size = min;
        lst_digi a = lst_digi_init();
        std::list<DIGI> b;
        for(size_t pushes = 0; pushes < size; pushes++)
        {
            int value = rand() % INT_MAX;
            lst_digi_push_back(&a, digi_init(value));
            b.push_back(DIGI{value});
        }
#define LIST          \
        X(PUSH_BACK)  \
        X(PUSH_FRONT) \
        X(ERASE)      \
        X(INSERT)     \
        X(CLEAR)      \
        X(RESIZE)     \
        X(ASSIGN)     \
        X(COPY)       \
        X(SWAP)       \
        X(REVERSE)    \
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
            case ERASE:
            {
                size_t index = rand() % a.size;
                size_t current = 0;
                std::list<DIGI>::iterator iter = b.begin();
                lst_digi_it it = lst_digi_it_each(&a);
                CTL_FOR(it,
                {
                    if(current == index)
                    {
                        lst_digi_erase(&a, it.node);
                        b.erase(iter);
                        break;
                    }
                    iter++;
                    current += 1;
                });
                break;
            }
            case INSERT:
            {
                size_t index = rand() % a.size;
                int value = rand() % INT_MAX;
                size_t current = 0;
                std::list<DIGI>::iterator iter = b.begin();
                lst_digi_it it = lst_digi_it_each(&a);
                CTL_FOR(it,
                {
                    if(current == index)
                    {
                        lst_digi_insert(&a, it.node, digi_init(value));
                        b.insert(iter, DIGI{value});
                        break;
                    }
                    iter++;
                    current += 1;
                });
                break;
            }
            case CLEAR:
            {
                lst_digi_clear(&a);
                b.clear();
                break;
            }
            case RESIZE:
            {
                size_t resize = rand() % (3 * a.size);
                lst_digi_resize(&a, resize);
                b.resize(resize);
                break;
            }
            case ASSIGN:
            {
                size_t width = rand() % a.size;
                if(width > 2)
                {
                    int value = rand() % INT_MAX;
                    lst_digi_assign(&a, width, digi_init(value));
                    b.assign(width, DIGI{value});
                }
                break;
            }
            case COPY:
            {
                lst_digi aa = lst_digi_copy(&a);
                std::list<DIGI> bb = b;
                test_equal(&aa, bb);
                lst_digi_free(&aa);
                break;
            }
            case SWAP:
            {
                lst_digi aa = lst_digi_copy(&a);
                lst_digi aaa;
                std::list<DIGI> bb = b;
                std::list<DIGI> bbb;
                lst_digi_swap(&aaa, &aa);
                std::swap(bb, bbb);
                test_equal(&aaa, bbb);
                lst_digi_free(&aaa);
                break;
            }
            case REVERSE:
            {
                lst_digi_reverse(&a);
                b.reverse();
                break;
            }
        }
        test_equal(&a, b);
        lst_digi_free(&a);
    }
    TEST_PASS(__FILE__);
}
