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

static bool
digi_is_odd(digi* d)
{
    return *d->value % 2;
}

static bool
digi_match(digi* a, digi* b)
{
    return *a->value == *b->value;
}

static bool
DIGI_is_odd(DIGI& d)
{
    return *d.value % 2;
}

static void
setup_lists(lst_digi* a, std::list<DIGI>& b, size_t size, int* max)
{
    *a = lst_digi_init();
    size_t min = 2;
    if(size < min)
        size = min;
    for(size_t pushes = 0; pushes < size; pushes++)
    {
        int value = rand() % (INT_MAX - 1);
        if(max && value > *max)
            *max = value;
        lst_digi_push_back(a, digi_init(value));
        b.push_back(DIGI{value});
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
        lst_digi a;
        std::list<DIGI> b;
        int max = 0;
        setup_lists(&a, b, rand() % TEST_MAX_SIZE, &max);
#define LIST          \
        X(PUSH_BACK)  \
        X(PUSH_FRONT) \
        X(POP_BACK)   \
        X(POP_FRONT)  \
        X(ERASE)      \
        X(INSERT)     \
        X(CLEAR)      \
        X(RESIZE)     \
        X(ASSIGN)     \
        X(SWAP)       \
        X(COPY)       \
        X(REVERSE)    \
        X(REMOVE_IF)  \
        X(SPLICE)     \
        X(MERGE)      \
        X(EQUAL)      \
        X(SORT)       \
        X(UNIQUE)     \
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
            case POP_FRONT:
            {
                lst_digi_pop_front(&a);
                b.pop_front();
                break;
            }
            case POP_BACK:
            {
                lst_digi_pop_back(&a);
                b.pop_back();
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
            case COPY:
            {
                lst_digi aa = lst_digi_copy(&a);
                std::list<DIGI> bb = b;
                test_equal(&aa, bb);
                lst_digi_free(&aa);
                break;
            }
            case REVERSE:
            {
                lst_digi_reverse(&a);
                b.reverse();
                break;
            }
            case REMOVE_IF:
            {
                lst_digi_remove_if(&a, digi_is_odd);
                b.remove_if(DIGI_is_odd);
                break;
            }
            case SPLICE:
            {
                size_t index = rand() % a.size;
                size_t current = 0;
                std::list<DIGI>::iterator iter = b.begin();
                lst_digi_it it = lst_digi_it_each(&a);
                CTL_FOR(it,
                {
                    if(current == index)
                        break;
                    iter++;
                    current += 1;
                });
                lst_digi aa;
                std::list<DIGI> bb;
                setup_lists(&aa, bb, rand() % TEST_MAX_SIZE, NULL);
                b.splice(iter, bb);
                lst_digi_splice(&a, it.node, &aa);
                break;
            }
            case MERGE:
            {
                lst_digi aa = lst_digi_init();
                std::list<DIGI> bb;
                size_t size = rand() % TEST_MAX_SIZE;
                int total = 0;
                for(size_t pushes = 0; pushes < size; pushes++)
                {
                    // MAX + 1 ENSURES MERGE CAN APPEND TO TAIL.
                    int value = rand() % 128;
                    total += value;
                    if(pushes == (size - 1))
                        total = max + 1;
                    lst_digi_push_back(&aa, digi_init(total));
                    bb.push_back(DIGI{total});
                }
                lst_digi_merge(&a, &aa, digi_compare);
                b.merge(bb);
                break;
            }
            case EQUAL:
            {
                lst_digi aa = lst_digi_copy(&a);
                std::list<DIGI> bb = b;
                assert(lst_digi_equal(&a, &aa, digi_match));
                assert(b == bb);
                lst_digi_free(&aa);
                break;
            }
            case SORT:
            {
                lst_digi_sort(&a, digi_compare);
                b.sort();
                break;
            }
            case UNIQUE:
            {
                lst_digi_unique(&a, digi_match);
                b.unique();
                break;
            }
        }
        test_equal(&a, b);
        lst_digi_free(&a);
    }
    TEST_PASS(__FILE__);
}
