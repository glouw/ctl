#include "../test.h"
#include "digi.hh"

static inline int
digi_key_compare(digi* a, digi* b)
{
    return (*a->value == *b->value) ? 0 : (*a->value < *b->value) ? -1 : 1;
}

#define USE_INTERNAL_VERIFY
#define T digi
#include <set.h>

#include <set>
#include <algorithm>

#define CHECK(_x, _y) {                           \
    assert(_x.size == _y.size());                 \
    std::set<DIGI>::iterator _iter = _y.begin();  \
    foreach(set_digi, &_x, _it) {                 \
        assert(*_it.ref->value == *_iter->value); \
        _iter++;                                  \
    }                                             \
    set_digi_it _it = set_digi_it_each(&_x);      \
    for(auto& _d : _y) {                          \
        assert(*_it.ref->value == *_d.value);     \
        _it.step(&_it);                           \
    }                                             \
}

static void
setup_sets(set_digi* a, std::set<DIGI>& b)
{
    size_t iters = TEST_RAND(TEST_MAX_SIZE);
    *a = set_digi_init(digi_key_compare);
    for(size_t inserts = 0; inserts < iters; inserts++)
    {
        const int vb = TEST_RAND(TEST_MAX_SIZE);
        set_digi_insert(a, digi_init(vb));
        b.insert(DIGI(vb));
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
        set_digi a;
        std::set<DIGI> b;
        setup_sets(&a, b);
        enum
        {
            TEST_SELF,
            TEST_INSERT,
            TEST_ERASE,
            TEST_REMOVE_IF,
            TEST_CLEAR,
            TEST_SWAP,
            TEST_COUNT,
            TEST_FIND,
            TEST_LOWER_BOUND,
            TEST_UPPER_BOUND,
            TEST_COPY,
            TEST_EQUAL,
            TEST_UNION,
            TEST_INTERSECTION,
            TEST_SYMMETRIC_DIFFERENCE,
            TEST_DIFFERENCE,
            TEST_TOTAL,
        };
        int which = TEST_RAND(TEST_TOTAL);
        switch(which)
        {
            case TEST_SELF:
            {
                set_digi aa = set_digi_copy(&a);
                foreach(set_digi, &aa, it)
                    assert(set_digi_find(&a, *it.ref));
                foreach(set_digi, &a, it)
                    set_digi_erase(&aa, *it.ref);
                assert(set_digi_empty(&aa));
                set_digi_free(&aa);
                CHECK(a, b);
                break;
            }
            case TEST_INSERT:
            {
                const int vb = TEST_RAND(TEST_MAX_SIZE);
                set_digi_insert(&a, digi_init(vb));
                b.insert(DIGI(vb));
                CHECK(a, b);
                break;
            }
            case TEST_ERASE:
            {
                const size_t erases = TEST_RAND(TEST_MAX_SIZE) / 4;
                for(size_t i = 0; i < erases; i++)
                    if(a.size > 0)
                    {
                        const int key = TEST_RAND(TEST_MAX_SIZE);
                        digi kd = digi_init(key);
                        set_digi_erase(&a, kd);
                        b.erase(DIGI(key));
                        CHECK(a, b);
                        digi_free(&kd);
                    }
                CHECK(a, b);
                break;
            }
            case TEST_REMOVE_IF:
            {
                size_t b_erases = 0;
                {   // C++20 STD::ERASE_IF
                    auto iter = b.begin();
                    auto end = b.end();
                    while(iter != end)
                    {
                        if((int) *iter->value % 2)
                        {
                            iter = b.erase(iter);
                            b_erases += 1;
                        }
                        else
                            iter++;
                    }
                }
                size_t a_erases = set_digi_remove_if(&a, digi_is_odd);
                assert(a_erases == b_erases);
                CHECK(a, b);
                break;
            }
            case TEST_CLEAR:
            {
                b.clear();
                set_digi_clear(&a);
                CHECK(a, b);
                break;
            }
            case TEST_SWAP:
            {
                set_digi aa = set_digi_copy(&a);
                set_digi aaa = set_digi_init(digi_key_compare);
                std::set<DIGI> bb = b;
                std::set<DIGI> bbb;
                set_digi_swap(&aaa, &aa);
                std::swap(bb, bbb);
                CHECK(aaa, bbb);
                set_digi_free(&aaa);
                CHECK(a, b);
                break;
            }
            case TEST_COUNT:
            {
                int key = TEST_RAND(TEST_MAX_SIZE);
                digi kd = digi_init(key);
                int aa = set_digi_count(&a, kd);
                int bb = b.count(DIGI(key));
                assert(aa == bb);
                CHECK(a, b);
                digi_free(&kd);
                break;
            }
            case TEST_FIND:
            {
                int key = TEST_RAND(TEST_MAX_SIZE);
                digi kd = digi_init(key);
                set_digi_node* aa = set_digi_find(&a, kd);
                auto bb = b.find(DIGI(key));
                if(bb == b.end())
                    assert(set_digi_end(&a) == aa);
                else
                    assert(*bb->value == *aa->key.value);
                CHECK(a, b);
                digi_free(&kd);
                break;
            }
            case TEST_LOWER_BOUND:
            {
                int key = TEST_RAND(TEST_MAX_SIZE);
                digi kd = digi_init(key);
                set_digi_node* aa = set_digi_lower_bound(&a, kd);
                auto bb = b.lower_bound(DIGI(key));
                if(bb == b.end())
                    assert(set_digi_end(&a) == aa);
                else
                    assert(*bb->value == *aa->key.value);
                CHECK(a, b);
                digi_free(&kd);
                break;
            }
            case TEST_UPPER_BOUND:
            {
                int key = TEST_RAND(TEST_MAX_SIZE);
                digi kd = digi_init(key);
                set_digi_node* aa = set_digi_upper_bound(&a, kd);
                auto bb = b.upper_bound(DIGI(key));
                if(bb == b.end())
                    assert(set_digi_end(&a) == aa);
                else
                    assert(*bb->value == *aa->key.value);
                CHECK(a, b);
                digi_free(&kd);
                break;
            }
            case TEST_COPY:
            {
                set_digi aa = set_digi_copy(&a);
                std::set<DIGI> bb = b;
                CHECK(aa, bb);
                set_digi_free(&aa);
                CHECK(a, b);
                break;
            }
            case TEST_EQUAL:
            {
                set_digi aa = set_digi_copy(&a);
                std::set<DIGI> bb = b;
                assert(set_digi_equal(&a, &aa, digi_match));
                assert(b == bb);
                set_digi_free(&aa);
                CHECK(a, b);
                break;
            }
            case TEST_UNION:
            {
                set_digi aa;
                std::set<DIGI> bb;
                setup_sets(&aa, bb);
                set_digi aaa = set_digi_union(&a, &aa);
                std::set<DIGI> bbb;
                std::set_union(b.begin(), b.end(), bb.begin(), bb.end(), std::inserter(bbb, bbb.begin()));
                CHECK(a, b);
                CHECK(aa, bb);
                CHECK(aaa, bbb);
                set_digi_free(&aa);
                set_digi_free(&aaa);
                break;
            }
            case TEST_INTERSECTION:
            {
                set_digi aa;
                std::set<DIGI> bb;
                setup_sets(&aa, bb);
                set_digi aaa = set_digi_intersection(&a, &aa);
                std::set<DIGI> bbb;
                std::set_intersection(b.begin(), b.end(), bb.begin(), bb.end(), std::inserter(bbb, bbb.begin()));
                CHECK(a, b);
                CHECK(aa, bb);
                CHECK(aaa, bbb);
                set_digi_free(&aa);
                set_digi_free(&aaa);
                break;
            }
            case TEST_SYMMETRIC_DIFFERENCE:
            {
                set_digi aa;
                std::set<DIGI> bb;
                setup_sets(&aa, bb);
                set_digi aaa = set_digi_symmetric_difference(&a, &aa);
                std::set<DIGI> bbb;
                std::set_symmetric_difference(b.begin(), b.end(), bb.begin(), bb.end(), std::inserter(bbb, bbb.begin()));
                CHECK(a, b);
                CHECK(aa, bb);
                CHECK(aaa, bbb);
                set_digi_free(&aa);
                set_digi_free(&aaa);
                break;
            }
            case TEST_DIFFERENCE:
            {
                set_digi aa;
                std::set<DIGI> bb;
                setup_sets(&aa, bb);
                set_digi aaa = set_digi_difference(&a, &aa);
                std::set<DIGI> bbb;
                std::set_difference(b.begin(), b.end(), bb.begin(), bb.end(), std::inserter(bbb, bbb.begin()));
                CHECK(a, b);
                CHECK(aa, bb);
                CHECK(aaa, bbb);
                set_digi_free(&aa);
                set_digi_free(&aaa);
                break;
            }
        }
        CHECK(a, b);
        set_digi_free(&a);
    }
    TEST_PASS(__FILE__);
}
