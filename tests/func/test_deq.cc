#include "../test.h"
#include "digi.hh"

#define T digi
#include <deq.h>

#include <deque>
#include <algorithm>

#define CHECK(_x, _y) {                                           \
    assert(_x.size == _y.size());                                 \
    assert(deq_digi_empty(&_x) == _y.empty());                    \
    if(_x.size > 0) {                                             \
        assert(*_y.front().value == *deq_digi_front(&_x)->value); \
        assert(*_y.back().value == *deq_digi_back(&_x)->value);   \
    }                                                             \
    std::deque<DIGI>::iterator _iter = _y.begin();                \
    foreach(deq_digi, &_x, _it) {                                 \
        assert(*_it.ref->value == *_iter->value);                 \
        _iter++;                                                  \
    }                                                             \
    deq_digi_it _it = deq_digi_it_each(&_x);                      \
    for(auto& _d : _y) {                                          \
        assert(*_it.ref->value == *_d.value);                     \
        _it.step(&_it);                                           \
    }                                                             \
    for(size_t i = 0; i < _y.size(); i++)                         \
        assert(*_y.at(i).value == *deq_digi_at(&_x, i)->value);   \
}

// TESTS DEQ STABILITY WITH SELF CLEANUP.
// EDGE CASE:
//     MISC CALLS TO POP/PUSH FRONT/BACK WITH
//     DEQUE SIZE 1 CAUSED HEAP USE AFTER FREE ISSUES.

void
test_capacity_edge_case(void)
{
    {
        deq_digi a = deq_digi_init();
        deq_digi_push_back(&a, digi_init(1));
        assert(a.capacity == 1);
        deq_digi_pop_back(&a);
        assert(a.capacity == 1);
        deq_digi_push_back(&a, digi_init(1));
        assert(a.capacity == 1);
        deq_digi_pop_back(&a);
        assert(a.capacity == 1);
        deq_digi_free(&a);
    }{
        deq_digi a = deq_digi_init();
        deq_digi_push_back(&a, digi_init(1));
        assert(a.capacity == 1);
        deq_digi_pop_front(&a);
        assert(a.capacity == 1);
        deq_digi_push_back(&a, digi_init(1));
        assert(a.capacity == 1);
        deq_digi_pop_front(&a);
        assert(a.capacity == 1);
        deq_digi_free(&a);
    }{
        deq_digi a = deq_digi_init();
        deq_digi_push_front(&a, digi_init(1));
        assert(a.capacity == 1);
        deq_digi_pop_front(&a);
        assert(a.capacity == 1);
        deq_digi_push_front(&a, digi_init(1));
        assert(a.capacity == 1);
        deq_digi_pop_front(&a);
        assert(a.capacity == 1);
        deq_digi_free(&a);
    }{
        deq_digi a = deq_digi_init();
        deq_digi_push_front(&a, digi_init(1));
        assert(a.capacity == 1);
        deq_digi_pop_back(&a);
        assert(a.capacity == 1);
        deq_digi_push_front(&a, digi_init(1));
        assert(a.capacity == 1);
        deq_digi_pop_back(&a);
        assert(a.capacity == 1);
        deq_digi_free(&a);
    }
}
void
test_random_work_load(void)
{
    deq_digi a = deq_digi_init();
    std::deque<DIGI> b;
    const size_t loops = TEST_RAND(TEST_MAX_LOOPS);
    for(size_t i = 0; i < loops; i++)
        switch(TEST_RAND(5))
        {
            case 0:
            {
                assert(a.size == b.size());
                deq_digi_push_front(&a, digi_init(1));
                b.push_front(DIGI(1));
                assert(a.size == b.size());
                break;
            }
            case 1:
            {
                assert(a.size == b.size());
                deq_digi_push_back(&a, digi_init(1));
                b.push_back(DIGI(1));
                assert(a.size == b.size());
                break;
            }
            case 2:
            {
                assert(a.size == b.size());
                if(a.size)
                    deq_digi_pop_front(&a);
                if(b.size())
                    b.pop_front();
                assert(a.size == b.size());
                break;
            }
            case 3:
            {
                assert(a.size == b.size());
                if(a.size)
                    deq_digi_pop_back(&a);
                if(b.size())
                    b.pop_back();
                assert(a.size == b.size());
                break;
            }
            case 4:
            {
                assert(a.size == b.size());
                deq_digi_clear(&a);
                b.clear();
                assert(a.size == b.size());
                break;
            }
        }
    deq_digi_free(&a);
}

int
main(void)
{
    test_capacity_edge_case();
    test_random_work_load();
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
            deq_digi a = deq_digi_init();
            std::deque<DIGI> b;
            if(mode == MODE_DIRECT)
            {
                deq_digi_resize(&a, size, digi_init(0));
                b.resize(size);
            }
            if(mode == MODE_GROWTH)
            {
                for(size_t pushes = 0; pushes < size; pushes++)
                {
                    const int value = TEST_RAND(INT_MAX);
                    deq_digi_push_back(&a, digi_init(value));
                    b.push_back(DIGI(value));
                }
            }
            enum
            {
                TEST_PUSH_BACK,
                TEST_POP_BACK,
                TEST_PUSH_FRONT,
                TEST_POP_FRONT,
                TEST_CLEAR,
                TEST_ERASE,
                TEST_RESIZE,
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
                    deq_digi_push_back(&a, digi_init(value));
                    CHECK(a, b);
                    break;
                }
                case TEST_POP_BACK:
                {
                    if(a.size > 0)
                    {
                        b.pop_back();
                        deq_digi_pop_back(&a);
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_PUSH_FRONT:
                {
                    const int value = TEST_RAND(INT_MAX);
                    b.push_front(DIGI(value));
                    deq_digi_push_front(&a, digi_init(value));
                    CHECK(a, b);
                    break;
                }
                case TEST_POP_FRONT:
                {
                    if(a.size > 0)
                    {
                        b.pop_front();
                        deq_digi_pop_front(&a);
                        CHECK(a, b);
                    }
                    break;
                }
                case TEST_CLEAR:
                {
                    b.clear();
                    deq_digi_clear(&a);
                    CHECK(a, b);
                    break;
                }
                case TEST_ERASE:
                {
                    if(a.size > 0)
                    {
                        const size_t index = TEST_RAND(a.size);
                        b.erase(b.begin() + index);
                        deq_digi_erase(&a, index);
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_RESIZE:
                {
                    const size_t resize = 3 * TEST_RAND(a.size) + 1;
                    b.resize(resize);
                    deq_digi_resize(&a, resize, digi_init(0));
                    CHECK(a, b);
                    break;
                }
                case TEST_SORT:
                {
                    deq_digi_sort(&a, digi_compare);
                    std::sort(b.begin(), b.end());
                    CHECK(a, b);
                    break;
                }
                case TEST_COPY:
                {
                    deq_digi aa = deq_digi_copy(&a);
                    std::deque<DIGI> bb = b;
                    CHECK(aa, bb);
                    deq_digi_free(&aa);
                    CHECK(a, b);
                    break;
                }
                case TEST_SWAP:
                {
                    deq_digi aa = deq_digi_copy(&a);
                    deq_digi aaa = deq_digi_init();
                    std::deque<DIGI> bb = b;
                    std::deque<DIGI> bbb;
                    deq_digi_swap(&aaa, &aa);
                    std::swap(bb, bbb);
                    CHECK(aaa, bbb);
                    deq_digi_free(&aaa);
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
                        deq_digi_insert(&a, index, digi_init(value));
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_ASSIGN:
                {
                    const int value = TEST_RAND(INT_MAX);
                    size_t assign_size = TEST_RAND(a.size) + 1;
                    deq_digi_assign(&a, assign_size, digi_init(value));
                    b.assign(assign_size, DIGI(value));
                    CHECK(a, b);
                    break;
                }
                case TEST_REMOVE_IF:
                {
                    deq_digi_remove_if(&a, digi_is_odd);
                    b.erase(std::remove_if(b.begin(), b.end(), DIGI_is_odd), b.end());
                    CHECK(a, b);
                    break;
                }
                case TEST_EQUAL:
                {
                    deq_digi aa = deq_digi_copy(&a);
                    std::deque<DIGI> bb = b;
                    assert(deq_digi_equal(&a, &aa, digi_match));
                    assert(b == bb);
                    deq_digi_free(&aa);
                    CHECK(a, b);
                    break;
                }
                case TEST_FIND:
                {
                    if(a.size > 0)
                    {
                        const size_t index = TEST_RAND(a.size);
                        int value = TEST_RAND(2) ? TEST_RAND(INT_MAX) : *deq_digi_at(&a, index)->value;
                        digi key = digi_init(value);
                        digi* aa = deq_digi_find(&a, key, digi_match);
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
            deq_digi_free(&a);
        }
    }
    TEST_PASS(__FILE__);
}
