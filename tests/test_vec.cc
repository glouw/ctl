#include "test.h"
#include "digi.h"

#define CTL_T digi
#include <vec.h>

#include <vector>
#include <algorithm>

// EXPANDED INLINE SO ASSERTS CAN CATCH SWITCH STATEMENT PROBLEMS.

#define CHECK(_a, _b) {                                 \
    {                                                   \
        for(auto _d : _b)                               \
            assert(_d.value != nullptr);                \
        for(size_t i = 0; i < _a.size; i++)             \
            assert(vec_digi_at(&_a, i) != NULL);        \
    }{                                                  \
        size_t index = 0;                               \
        for(auto& _d : _b)                              \
        {                                               \
            digi* _ref = vec_digi_at(&_a, index);       \
            assert(*_d.value == *_ref->value);          \
            index += 1;                                 \
        }                                               \
    }{                                                  \
        std::vector<DIGI>::iterator _iter = _b.begin(); \
        vec_digi_it _it = vec_digi_it_each(&_a);        \
        CTL_FOR(_it, {                                  \
            assert(*_it.ref->value == *_iter->value);   \
            _iter++;                                    \
        });                                             \
    }{                                                  \
        assert(_a.capacity == _b.capacity());           \
        assert(_a.size == _b.size());                   \
        assert(vec_digi_empty(&_a) == _b.empty());      \
    }{                                                  \
        if(_a.size > 0)                                 \
        {                                               \
            assert(&_a.value[0] == vec_digi_data(&_a)); \
            assert(&_b[0] == _b.data());                \
            DIGI& _bf = _b.front();                     \
            DIGI& _bb = _b.back();                      \
            digi* _af = vec_digi_front(&_a);            \
            digi* _ab = vec_digi_back(&_a);             \
            assert(*_af->value == *_bf.value);          \
            assert(*_ab->value == *_bb.value);          \
        }                                               \
    }                                                   \
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
                vec_digi_resize(&a, size);
                b.resize(size);
            }
            if(mode == MODE_GROWTH)
            {
                for(size_t pushes = 0; pushes < size; pushes++)
                {
                    const int value = TEST_RAND(INT_MAX);
                    vec_digi_push_back(&a, digi_init(value));
                    b.push_back(DIGI{value});
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
                TEST_TOTAL,
            };
            int which = TEST_RAND(TEST_TOTAL);
            switch(which)
            {
                case TEST_PUSH_BACK:
                {
                    const int value = TEST_RAND(INT_MAX);
                    b.push_back(DIGI{value});
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
                        vec_digi_erase(&a, vec_digi_begin(&a) + index);
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
                        b.insert(b.begin() + index, DIGI{value});
                        vec_digi_insert(&a, vec_digi_begin(&a) + index, digi_init(value));
                    }
                    CHECK(a, b);
                    break;
                }
                case TEST_RESIZE:
                {
                    const size_t resize = 3 * TEST_RAND(a.size) + 1;
                    b.resize(resize);
                    vec_digi_resize(&a, resize);
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
                    digi d = digi_init(value);
                    vec_digi_assign(&a, assign_size, d);
                    b.assign(assign_size, DIGI{value});
                    CHECK(a, b);
                    break;
                }
                case TEST_SWAP:
                {
                    vec_digi aa = vec_digi_copy(&a);
                    vec_digi aaa;
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
            }
            CHECK(a, b);
            vec_digi_free(&a);
        }
    }
    TEST_PASS(__FILE__);
}
