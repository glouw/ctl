#include "test.h"
#include "digi.h"

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
    foreach(deq_digi, &_x, _it, {                                 \
        assert(*_it.ref->value == *_iter->value);                 \
        _iter++;                                                  \
    });                                                           \
    deq_digi_it _it = deq_digi_it_each(&_x);                      \
    for(auto& _d : _y) {                                          \
        assert(*_it.ref->value == *_d.value);                     \
        _it.step(&_it);                                           \
    }                                                             \
    for(size_t i = 0; i < _y.size(); i++)                         \
        assert(*_y.at(i).value == *deq_digi_at(&_x, i)->value);   \
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
            deq_digi a = deq_digi_init();
            std::deque<DIGI> b;
            if(mode == MODE_DIRECT)
            {
                deq_digi_resize(&a, size);
                b.resize(size);
            }
            if(mode == MODE_GROWTH)
            {
                for(size_t pushes = 0; pushes < size; pushes++)
                {
                    const int value = TEST_RAND(INT_MAX);
                    deq_digi_push_back(&a, digi_init(value));
                    b.push_back(DIGI{value});
                }
            }
            enum
            {
                TEST_PUSH_BACK,
                TEST_POP_BACK,
                TEST_PUSH_FRONT,
                TEST_POP_FRONT,
                TEST_TOTAL,
            };
            int which = TEST_RAND(TEST_TOTAL);
            switch(which)
            {
                case TEST_PUSH_BACK:
                {
                    const int value = TEST_RAND(INT_MAX);
                    b.push_back(DIGI{value});
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
                    b.push_front(DIGI{value});
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
            }
            CHECK(a, b);
            deq_digi_free(&a);
        }
    }
    TEST_PASS(__FILE__);
}
