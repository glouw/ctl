#include "test.h"

#include "digi.cc"

#define CTL_T digi
#include <vec.h>

#include <vector>
#include <algorithm>

static void
test_equal(vec_digi* a, std::vector<DIGI>& b)
{
    {
        for(auto d : b)
            assert(d.value != nullptr);
        for(size_t i = 0; i < a->size; i++)
            assert(vec_digi_at(a, i) != NULL);
    }{
        size_t index = 0;
        for(auto& d : b)
        {
            digi* ref = vec_digi_at(a, index);
            assert(*d.value == *ref->value);
            index += 1;
        }
    }{
        std::vector<DIGI>::iterator iter = b.begin();
        vec_digi_it it = vec_digi_it_each(a);
        CTL_FOR(it, {
            assert(*it.ref->value == *iter->value);
            iter++;
        });
    }{
        assert(a->capacity == b.capacity());
        assert(a->size == b.size());
        assert(vec_digi_empty(a) == b.empty());
    }{
        assert(&a->value[0] == vec_digi_data(a));
        assert(&b[0] == b.data());
    }{
        if(a->size > 0)
        {
            DIGI& bf = b.front();
            DIGI& bb = b.back();
            digi* af = vec_digi_front(a);
            digi* ab = vec_digi_back(a);
            assert(*af->value == *bf.value);
            assert(*ab->value == *bb.value);
        }
    }
}

int
main(void)
{
#ifdef SRAND
    srand(time(NULL));
#endif
    const size_t loops = rand() % TEST_MAX_LOOPS;
    for(size_t loop = 0; loop < loops; loop++)
    {
        size_t size = rand() % TEST_MAX_SIZE;
        if(size == 0)
            size = 1;
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
                    const int value = rand() % INT_MAX;
                    vec_digi_push_back(&a, digi_init(value));
                    b.push_back(DIGI{value});
                }
            }
            enum
            {
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
                TEST_TOTAL,
            };
            switch(rand() % TEST_TOTAL)
            {
                case TEST_CLEAR:
                {
                    b.clear();
                    vec_digi_clear(&a);
                    break;
                }
                case TEST_ERASE:
                {
                    const size_t index = rand() % a.size;
                    b.erase(b.begin() + index);
                    vec_digi_erase(&a, index);
                    break;
                }
                case TEST_INSERT:
                {
                    size_t amount = rand() % 512;
                    for(size_t count = 0; count < amount; count++)
                    {
                        const int value = rand() % INT_MAX;
                        const size_t index = rand() % a.size;
                        b.insert(b.begin() + index, DIGI{value});
                        vec_digi_insert(&a, index, digi_init(value));
                    }
                    break;
                }
                case TEST_RESIZE:
                {
                    const size_t resize = (size == 0) ? 0 : (rand() % (size * 3));
                    b.resize(resize);
                    vec_digi_resize(&a, resize);
                    break;
                }
                case TEST_RESERVE:
                {
                    const size_t capacity = (a.capacity == 0) ? 0 : (rand() % (a.capacity * 2));
                    b.reserve(capacity);
                    vec_digi_reserve(&a, capacity);
                    break;
                }
                case TEST_SHRINK_TO_FIT:
                {
                    b.shrink_to_fit();
                    vec_digi_shrink_to_fit(&a);
                    break;
                }
                case TEST_SORT:
                {
                    vec_digi_sort(&a, digi_compare);
                    std::sort(b.begin(), b.end());
                    break;
                }
                case TEST_COPY:
                {
                    vec_digi aa = vec_digi_copy(&a);
                    std::vector<DIGI> bb = b;
                    test_equal(&aa, bb);
                    vec_digi_free(&aa);
                    break;
                }
                case TEST_ASSIGN:
                {
                    const int value = rand() % INT_MAX;
                    size_t assign_size = rand() % a.size;
                    if(assign_size == 0)
                        assign_size = 1;
                    vec_digi_assign(&a, assign_size, digi_init(value));
                    b.assign(assign_size, DIGI{value});
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
                    test_equal(&aaa, bbb);
                    vec_digi_free(&aaa);
                    break;
                }
            }
            test_equal(&a, b);
            vec_digi_free(&a);
        }
    }
    TEST_PASS(__FILE__);
}
