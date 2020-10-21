#include "test.h"

#include "digi.cc"

#define CTL_T digi
#include <lst.h>

#include <list>

static void
test_equal(lst_digi* a, std::list<DIGI>& b)
{
    assert(a->size == b.size());
    if(a->size > 0 && b.size() > 0)
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
digi_match(digi* a, digi* b)
{
    return *a->value == *b->value;
}

static void
setup_lists(lst_digi* a, std::list<DIGI>& b, size_t size, int* max_value)
{
    *a = lst_digi_init();
    for(size_t pushes = 0; pushes < size; pushes++)
    {
        int value = TEST_RAND(INT_MAX - 1); // SEE COMMENT IN CASE MERGE.
        if(max_value && value > *max_value)
            *max_value = value;
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
    const size_t loops = TEST_RAND(TEST_MAX_LOOPS);
    for(size_t loop = 0; loop < loops; loop++)
    {
        lst_digi a;
        std::list<DIGI> b;
        int max_value = 0;
        setup_lists(&a, b, TEST_RAND(TEST_MAX_SIZE), &max_value);
        enum
        {
            TEST_PUSH_BACK,
            TEST_PUSH_FRONT,
            TEST_POP_BACK,
            TEST_POP_FRONT,
            TEST_ERASE,
            TEST_INSERT,
            TEST_CLEAR,
            TEST_RESIZE,
            TEST_ASSIGN,
            TEST_SWAP,
            TEST_COPY,
            TEST_REVERSE,
            TEST_REMOVE_IF,
            TEST_SPLICE,
            TEST_MERGE,
            TEST_EQUAL,
            TEST_SORT,
            TEST_UNIQUE,
            TEST_TOTAL
        };
        int which = TEST_RAND(TEST_TOTAL);
        switch(which)
        {
            case TEST_PUSH_FRONT: // POINTER XOR ENSURE VALUE LOCATION DOES NOT CHANGE.
            {
                int value = TEST_RAND(INT_MAX);
                lst_digi_push_front(&a, digi_init(value));
                b.push_front(DIGI{value});
                break;
            }
            case TEST_PUSH_BACK:
            {
                int value = TEST_RAND(INT_MAX);
                lst_digi_push_back(&a, digi_init(value));
                b.push_back(DIGI{value});
                break;
            }
            case TEST_POP_FRONT:
            {
                if(a.size > 0)
                {
                    lst_digi_pop_front(&a);
                    b.pop_front();
                }
                break;
            }
            case TEST_POP_BACK:
            {
                if(a.size > 0)
                {
                    lst_digi_pop_back(&a);
                    b.pop_back();
                }
                break;
            }
            case TEST_ERASE:
            {
                size_t index = TEST_RAND(a.size);
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
            case TEST_INSERT:
            {
                size_t index = TEST_RAND(a.size);
                int value = TEST_RAND(INT_MAX);
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
            case TEST_CLEAR:
            {
                lst_digi_clear(&a);
                b.clear();
                break;
            }
            case TEST_RESIZE:
            {
                size_t resize = 3 * TEST_RAND(a.size);
                lst_digi_resize(&a, resize);
                b.resize(resize);
                break;
            }
            case TEST_ASSIGN:
            {
                size_t width = TEST_RAND(a.size);
                if(width > 2)
                {
                    int value = TEST_RAND(INT_MAX);
                    lst_digi_assign(&a, width, digi_init(value));
                    b.assign(width, DIGI{value});
                }
                break;
            }
            case TEST_SWAP:
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
            case TEST_COPY:
            {
                lst_digi aa = lst_digi_copy(&a);
                std::list<DIGI> bb = b;
                test_equal(&aa, bb);
                lst_digi_free(&aa);
                break;
            }
            case TEST_REVERSE:
            {
                lst_digi_reverse(&a);
                b.reverse();
                break;
            }
            case TEST_REMOVE_IF:
            {
                lst_digi_remove_if(&a, digi_is_odd);
                b.remove_if(DIGI_is_odd);
                break;
            }
            case TEST_SPLICE:
            {
                size_t index = TEST_RAND(a.size);
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
                setup_lists(&aa, bb, TEST_RAND(TEST_MAX_SIZE), NULL);
                b.splice(iter, bb);
                lst_digi_splice(&a, it.node, &aa);
                break;
            }
            case TEST_MERGE:
            {
                lst_digi aa = lst_digi_init();
                std::list<DIGI> bb;
                size_t size = TEST_RAND(TEST_MAX_SIZE);
                int total = 0;
                for(size_t pushes = 0; pushes < size; pushes++)
                {
                    int value = TEST_RAND(128);
                    total += value;
                    if(pushes == (size - 1))
                        total = max_value + 1; // MAX + 1 ENSURES MERGE CAN APPEND TO TAIL.
                    lst_digi_push_back(&aa, digi_init(total));
                    bb.push_back(DIGI{total});
                }
                b.merge(bb);
                lst_digi_merge(&a, &aa, digi_compare);
                break;
            }
            case TEST_EQUAL:
            {
                lst_digi aa = lst_digi_copy(&a);
                std::list<DIGI> bb = b;
                assert(lst_digi_equal(&a, &aa, digi_match));
                assert(b == bb);
                lst_digi_free(&aa);
                break;
            }
            case TEST_SORT:
            {
                lst_digi_sort(&a, digi_compare);
                b.sort();
                break;
            }
            case TEST_UNIQUE:
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
