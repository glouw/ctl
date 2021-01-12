#include "../test.h"
#include "digi.hh"

#define T digi
#include <ust.h>

#include <unordered_set>
#include <algorithm>

#define CHECK(_x, _y) {                                                \
    assert(_x.size == _y.size());                                      \
    if(_x.bucket_count > 0)                                            \
        assert(_x.bucket_count == _y.bucket_count());                  \
    if(_x.size > 0)                                                    \
    {                                                                  \
        size_t a_found = 0;                                            \
        size_t b_found = 0;                                            \
        foreach(ust_digi, &_x, it)                                     \
        {                                                              \
            auto found = _y.find(DIGI(*it.ref->value));                \
            assert(found != _y.end());                                 \
            a_found += 1;                                              \
        }                                                              \
        for(auto x : _y)                                               \
        {                                                              \
            digi d = digi_init(*x.value);                              \
            ust_digi_node* found = ust_digi_find(&_x, d);              \
            assert(found != NULL);                                     \
            digi_free(&d);                                             \
            b_found += 1;                                              \
        }                                                              \
        assert(a_found == b_found);                                    \
        for(size_t i = 0; i < _x.bucket_count; i++)                    \
            assert(ust_digi_bucket_size(&_x, i) == _y.bucket_size(i)); \
    }                                                                  \
}

static float
frand(void)
{
    return 1e6 * rand() / (float) RAND_MAX;
}

static void
setup_usts(ust_digi* a, std::unordered_set<DIGI>& b)
{
    size_t iters = TEST_RAND(TEST_MAX_SIZE);
    *a = ust_digi_init(digi_hash, digi_equal);
    for(size_t inserts = 0; inserts < iters; inserts++)
    {
        float f = frand();
        ust_digi_insert(a, digi_init(f));
        b.insert(DIGI(f));
    }
}

static void
test_small_size(void)
{
    ust_digi a = ust_digi_init(digi_hash, digi_equal);
    ust_digi_insert(&a, digi_init(1.0f));
    ust_digi_insert(&a, digi_init(2.0f));
    ust_digi_free(&a);
}

int
main(void)
{
#ifdef SRAND
    srand(time(NULL));
#endif
    test_small_size();
    const size_t loops = TEST_RAND(TEST_MAX_LOOPS);
    for(size_t loop = 0; loop < loops; loop++)
    {
        ust_digi a;
        std::unordered_set<DIGI> b;
        setup_usts(&a, b);
        CHECK(a, b);
        enum
        {
            TEST_SELF,
            TEST_FIND,
            TEST_ERASE,
            TEST_REMOVE_IF,
            TEST_INSERT,
            TEST_RESERVE,
            TEST_REHASH,
            TEST_CLEAR,
            TEST_SWAP,
            TEST_COUNT,
            TEST_COPY,
            TEST_EQUAL,
            TEST_TOTAL,
        };
        int which = TEST_RAND(TEST_TOTAL);
        switch(which)
        {
            case TEST_SELF:
            {
                ust_digi aa = ust_digi_copy(&a);
                foreach(ust_digi, &aa, it)
                    assert(ust_digi_find(&a, *it.ref));
                foreach(ust_digi, &a, it)
                    ust_digi_erase(&aa, *it.ref);
                assert(ust_digi_empty(&aa));
                ust_digi_free(&aa);
                CHECK(a, b);
                break;
            }
            case TEST_FIND:
            {
                int key = TEST_RAND(TEST_MAX_SIZE);
                digi kd = digi_init(key);
                ust_digi_node* aa = ust_digi_find(&a, kd);
                auto bb = b.find(DIGI(key));
                if(bb == b.end())
                    assert(ust_digi_end(&a) == aa);
                else
                    assert(*bb->value == *aa->key.value);
                CHECK(a, b);
                digi_free(&kd);
                break;
            }
            case TEST_ERASE:
            {
                const int temp = 42;
                ust_digi_insert(&a, digi_init(temp));
                b.insert(DIGI(temp));
                digi z = digi_init(temp);
                ust_digi_erase(&a, z);
                b.erase(DIGI(temp));
                digi_free(&z);
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
                size_t a_erases = ust_digi_remove_if(&a, digi_is_odd);
                assert(a_erases == b_erases);
                CHECK(a, b);
                break;
            }
            case TEST_INSERT:
            {
                const float vb = frand();
                ust_digi_insert(&a, digi_init(vb));
                b.insert(DIGI(vb));
                CHECK(a, b);
                break;
            }
            case TEST_RESERVE:
            {
                const size_t capacity = TEST_RAND(a.bucket_count) + 1;
                b.reserve(capacity);
                ust_digi_reserve(&a, capacity);
                CHECK(a, b);
                break;
            }
            case TEST_REHASH:
            {
                const size_t capacity = 3 * TEST_RAND(a.bucket_count) + 1;
                b.rehash(capacity);
                ust_digi_rehash(&a, capacity);
                CHECK(a, b);
                break;
            }
            case TEST_CLEAR:
            {
                b.clear();
                ust_digi_clear(&a);
                CHECK(a, b);
                break;
            }
            case TEST_SWAP:
            {
                ust_digi aa = ust_digi_copy(&a);
                ust_digi aaa = ust_digi_init(digi_hash, digi_equal);
                std::unordered_set<DIGI> bb = b;
                std::unordered_set<DIGI> bbb;
                ust_digi_swap(&aaa, &aa);
                std::swap(bb, bbb);
                CHECK(aaa, bbb);
                ust_digi_free(&aaa);
                CHECK(a, b);
                break;
            }
            case TEST_COUNT:
            {
                size_t which_index = TEST_RAND(a.size);
                size_t index = 0;
                float value = 0.3f;
                if(TEST_RAND(2))
                    foreach(ust_digi, &a, it)
                    {
                        if(index == which_index)
                        {
                            value = *it.ref->value;
                            break;
                        }
                        index += 1;
                    }
                digi kd = digi_init(value);
                int aa = ust_digi_count(&a, kd);
                int bb = b.count(DIGI(value));
                assert(aa == bb);
                CHECK(a, b);
                digi_free(&kd);
                break;
            }
            case TEST_COPY:
            {
                ust_digi aa = ust_digi_copy(&a);
                std::unordered_set<DIGI> bb = b;
                CHECK(aa, bb);
                ust_digi_free(&aa);
                CHECK(a, b);
                break;
            }
            case TEST_EQUAL:
            {
                ust_digi aa = ust_digi_copy(&a);
                std::unordered_set<DIGI> bb = b;
                assert(ust_digi_equal(&a, &aa));
                assert(b == bb);
                ust_digi_free(&aa);
                CHECK(a, b);
                break;
            }
        }
        CHECK(a, b);
        ust_digi_free(&a);
    }
    TEST_PASS(__FILE__);
}
