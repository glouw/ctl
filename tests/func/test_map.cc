#include "../test.h"
#include "digi.hh"

static inline void
int_digi_free(int* a, digi* b)
{
    (void) a;
    digi_free(b);
}

static inline void
int_digi_copy(int* a, int* b, digi* c, digi* d)
{
    *a = *b;
    *c = digi_copy(d);
}

static inline int
int_digi_equal(int* a, int* b, digi* c, digi* d)
{
    return *a == *b
        && *c->value == *d->value;
}

#define USE_INTERNAL_VERIFY
#define T int
#define U digi
#include <map.h>

#include <map>
#include <algorithm>

static inline int
int_key_compare(int* a, int* b)
{
    return (*a == *b) ? 0 : (*a < *b) ? -1 : 1;
}

static inline int
int_digi_is_value_odd(int* key, digi* value)
{
    (void) key;
    return *value->value % 2;
}

#define CHECK(_x, _y) {                                          \
    std::map<int, DIGI>::iterator _iter = _y.begin();            \
    foreach(map_int_digi, &_x, _it, {                            \
        assert(_it.node->first == _iter->first);                 \
        assert(*_it.node->second.value == *_iter->second.value); \
        _iter++;                                                 \
    });                                                          \
    map_int_digi_it _it = map_int_digi_it_each(&_x);             \
    for(auto& _d : _y) {                                         \
        assert(_it.node->first == _d.first);                     \
        assert(*_it.node->second.value == *_d.second.value);     \
        _it.step(&_it);                                          \
    }                                                            \
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
        size_t iters = TEST_RAND(TEST_MAX_SIZE);
        map_int_digi a = map_int_digi_init(int_key_compare);
        std::map<int, DIGI> b;
        for(size_t inserts = 0; inserts < iters; inserts++)
        {
            const int va = TEST_RAND(TEST_MAX_SIZE);
            const int vb = TEST_RAND(TEST_MAX_SIZE);
            map_int_digi_insert(&a, va, digi_init(vb));
            b.insert(std::pair<int, DIGI> { va, DIGI{vb} });
        }
        enum
        {
            TEST_INSERT,
            TEST_ERASE,
            TEST_REMOVE_IF,
            TEST_CLEAR,
            TEST_SWAP,
            TEST_COUNT,
            TEST_FIND,
            TEST_COPY,
            TEST_EQUAL,
            TEST_TOTAL,
        };
        int which = TEST_RAND(TEST_TOTAL);
        switch(which)
        {
            case TEST_INSERT:
            {
                const int va = TEST_RAND(TEST_MAX_SIZE);
                const int vb = TEST_RAND(TEST_MAX_SIZE);
                map_int_digi_insert(&a, va, digi_init(vb));
                b.insert(std::pair<int, DIGI> { va, DIGI{vb} });
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
                        map_int_digi_erase(&a, key);
                        b.erase(key);
                        CHECK(a, b);
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
                        if(*iter->second.value % 2)
                        {
                            iter = b.erase(iter);
                            b_erases += 1;
                        }
                        else
                            iter++;
                    }
                }
                size_t a_erases = map_int_digi_remove_if(&a, int_digi_is_value_odd);
                assert(a_erases == b_erases);
                CHECK(a, b);
                break;
            }
            case TEST_CLEAR:
            {
                b.clear();
                map_int_digi_clear(&a);
                CHECK(a, b);
                break;
            }
            case TEST_SWAP:
            {
                map_int_digi aa = map_int_digi_copy(&a);
                map_int_digi aaa = map_int_digi_init(int_key_compare);
                std::map<int, DIGI> bb = b;
                std::map<int, DIGI> bbb;
                map_int_digi_swap(&aaa, &aa);
                std::swap(bb, bbb);
                CHECK(aaa, bbb);
                map_int_digi_free(&aaa);
                CHECK(a, b);
                break;
            }
            case TEST_COUNT:
            {
                int key = TEST_RAND(TEST_MAX_SIZE);
                int aa = map_int_digi_count(&a, key );
                int bb = b.count(key );
                assert(aa == bb);
                CHECK(a, b);
                break;
            }
            case TEST_FIND:
            {
                int key = TEST_RAND(TEST_MAX_SIZE);
                map_int_digi_node* aa = map_int_digi_find(&a, key);
                auto bb = b.find(key);
                if(bb == b.end())
                    assert(map_int_digi_end(&a) == aa);
                else
                    assert(*bb->second.value == *aa->second.value);
                CHECK(a, b);
                break;
            }
#if 0
            case TEST_CONTAINS: // C++20.
            {
                int key = TEST_RAND(TEST_MAX_SIZE);
                int aa = map_int_digi_contains(&a, key);
                int bb = b.contains(key);
                assert(aa == bb);
                CHECK(a, b);
                break;
            }
#endif
            case TEST_COPY:
            {
                map_int_digi aa = map_int_digi_copy(&a);
                std::map<int, DIGI> bb = b;
                CHECK(aa, bb);
                map_int_digi_free(&aa);
                CHECK(a, b);
                break;
            }
            case TEST_EQUAL:
            {
                map_int_digi aa = map_int_digi_copy(&a);
                std::map<int, DIGI> bb = b;
                assert(map_int_digi_equal(&a, &aa, int_digi_equal));
                assert(b == bb);
                map_int_digi_free(&aa);
                CHECK(a, b);
                break;
            }
        }
        CHECK(a, b);
        map_int_digi_free(&a);
    }
    TEST_PASS(__FILE__);
}
