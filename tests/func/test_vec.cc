#include "../test.h"
#include "digi.hh"

#define T digi
#include <vec.h>

#include <vector>
#include <algorithm>

#define CHECK(_x, _y) {                                           \
    assert(_x.capacity == _y.capacity());                         \
    assert(_x.size == _y.size());                                 \
    assert(vec_digi_empty(&_x) == _y.empty());                    \
    if(_x.size > 0) {                                             \
        assert(*_y.front().value == *vec_digi_front(&_x)->value); \
        assert(*_y.back().value == *vec_digi_back(&_x)->value);   \
    }                                                             \
    std::vector<DIGI>::iterator _iter = _y.begin();               \
    foreach(vec_digi, &_x, _it) {                                 \
        assert(*_it.ref->value == *_iter->value);                 \
        _iter++;                                                  \
    }                                                             \
    vec_digi_it _it = vec_digi_it_each(&_x);                      \
    for(auto& _d : _y) {                                          \
        assert(*_it.ref->value == *_d.value);                     \
        _it.step(&_it);                                           \
    }                                                             \
    for(size_t i = 0; i < _y.size(); i++)                         \
        assert(*_y.at(i).value == *vec_digi_at(&_x, i)->value);   \
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
        enum
        {
            MODE_DIRECT,
            MODE_GROWTH,
            MODE_TOTAL
        };
        for(size_t mode = MODE_DIRECT; mode < MODE_TOTAL; mode++)
        {
            vec_digi a = vec_digi_init();
            std::vector<DIGI> b;
            if(mode == MODE_DIRECT)
            {
                vec_digi_resize(&a, size, digi_init(0));
                b.resize(size);
            }
            if(mode == MODE_GROWTH)
            {
                for(size_t pushes = 0; pushes < size; pushes++)
                {
                    const int value = TEST_RAND(INT_MAX);
                    vec_digi_push_back(&a, digi_init(value));
                    b.push_back(DIGI(value));
                }
            }
            enum
            {
                TEST_PUSH_BACK,
                TEST_POP_BACK,
                TEST_CLEAR,
                TEST_ERASE,
                TEST_RESIZE,
                TEST_RESERVE,
                TEST_SHRINK_TO_FIT,
                TEST_SORT,
                TEST_COPY,
                TEST_SWAP,
                TEST_INSERT,
                TEST_ASSIGN,
                TEST_REMOVE_IF,
                TEST_EQUAL,
                TEST_FIND,
                TEST_TOTAL,
            };
            int which = TEST_RAND(TEST_TOTAL);
            switch(which)
            {
                case TEST_PUSH_BACK:
                {
                    const int value = TEST_RAND(INT_MAX);
                    b.push_back(DIGI(value));
                    vec_digi_push_back(&a, digi_init(value));
                    CHECK(a, b);
                    break;
                }
                case TEST_POP_BACK:
                {
                    if(a.size > 0)
                    {
                        b.pop_back();
                        vec_digi_pop_back(&a);
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_CLEAR:
                {
                    b.clear();
                    vec_digi_clear(&a);
                    CHECK(a, b);
                    break;
                }
                case TEST_ERASE:
                {
                    if(a.size > 0)
                    {
                        const size_t index = TEST_RAND(a.size);
                        b.erase(b.begin() + index);
                        vec_digi_erase(&a, index);
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_INSERT:
                {
                    size_t amount = TEST_RAND(512);
                    for(size_t count = 0; count < amount; count++)
                    {
                        const int value = TEST_RAND(INT_MAX);
                        const size_t index = TEST_RAND(a.size);
                        b.insert(b.begin() + index, DIGI(value));
                        vec_digi_insert(&a, index, digi_init(value));
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_RESIZE:
                {
                    const size_t resize = 3 * TEST_RAND(a.size) + 1;
                    b.resize(resize);
                    vec_digi_resize(&a, resize, digi_init(0));
                    CHECK(a, b);
                    break;
                }
                case TEST_RESERVE:
                {
                    const size_t capacity = 3 * TEST_RAND(a.capacity) + 1;
                    b.reserve(capacity);
                    vec_digi_reserve(&a, capacity);
                    CHECK(a, b);
                    break;
                }
                case TEST_SHRINK_TO_FIT:
                {
                    b.shrink_to_fit();
                    vec_digi_shrink_to_fit(&a);
                    CHECK(a, b);
                    break;
                }
                case TEST_SORT:
                {
                    vec_digi_sort(&a, digi_compare);
                    std::sort(b.begin(), b.end());
                    CHECK(a, b);
                    break;
                }
                case TEST_COPY:
                {
                    vec_digi aa = vec_digi_copy(&a);
                    std::vector<DIGI> bb = b;
                    CHECK(aa, bb);
                    vec_digi_free(&aa);
                    CHECK(a, b);
                    break;
                }
                case TEST_ASSIGN:
                {
                    const int value = TEST_RAND(INT_MAX);
                    size_t assign_size = TEST_RAND(a.size) + 1;
                    vec_digi_assign(&a, assign_size, digi_init(value));
                    b.assign(assign_size, DIGI(value));
                    CHECK(a, b);
                    break;
                }
                case TEST_SWAP:
                {
                    vec_digi aa = vec_digi_copy(&a);
                    vec_digi aaa = vec_digi_init();
                    std::vector<DIGI> bb = b;
                    std::vector<DIGI> bbb;
                    vec_digi_swap(&aaa, &aa);
                    std::swap(bb, bbb);
                    CHECK(aaa, bbb);
                    vec_digi_free(&aaa);
                    CHECK(a, b);
                    break;
                }
                case TEST_REMOVE_IF:
                {
                    vec_digi_remove_if(&a, digi_is_odd);
                    b.erase(std::remove_if(b.begin(), b.end(), DIGI_is_odd), b.end());
                    CHECK(a, b);
                    break;
                }
                case TEST_EQUAL:
                {
                    vec_digi aa = vec_digi_copy(&a);
                    std::vector<DIGI> bb = b;
                    assert(vec_digi_equal(&a, &aa, digi_match));
                    assert(b == bb);
                    vec_digi_free(&aa);
                    CHECK(a, b);
                    break;
                }
                case TEST_FIND:
                {
                    if(a.size > 0)
                    {
                        const size_t index = TEST_RAND(a.size);
                        int value = TEST_RAND(2) ? TEST_RAND(INT_MAX) : *vec_digi_at(&a, index)->value;
                        digi key = digi_init(value);
                        digi* aa = vec_digi_find(&a, key, digi_match);
                        auto bb = std::find(b.begin(), b.end(), DIGI(value));
                        bool found_a = aa != NULL;
                        bool found_b = bb != b.end();
                        assert(found_a == found_b);
                        if(found_a && found_b)
                            assert(*aa->value == *bb->value);
                        digi_free(&key);
                        CHECK(a, b);
                    }
                    break;
                }
            }
            CHECK(a, b);
            vec_digi_free(&a);
        }
    }
    TEST_PASS(__FILE__);
}
