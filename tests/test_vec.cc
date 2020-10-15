#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <assert.h>

#include "test.h"

typedef struct
{
    int* value;
}
digi;

static digi
digi_init(int value)
{
    digi self = {
        (int*) malloc(sizeof(*self.value))
    };
    *self.value = value;
    return self;
}

static digi
digi_init_default(void)
{
    return digi_init(0);
}

static void
digi_free(digi* self)
{
    free(self->value);
}

static int
digi_compare(const void* a, const void* b)
{
    digi* aa = (digi*) a;
    digi* bb = (digi*) b;
    return *bb->value < *aa->value;
}

static digi
digi_copy(digi* self)
{
    digi copy = digi_init_default();
    *copy.value = *self->value;
    return copy;
}

#define CTL_T digi
#include <vec.h>

struct DIGI
{
    int* value;

    DIGI(int value): value { new int {value} }
    {
    }
    DIGI(): DIGI(0)
    {
    }
    ~DIGI()
    {
        delete value;
    }
    DIGI(const DIGI& a): DIGI()
    {
        *value = *a.value;
    }
    DIGI& operator=(const DIGI& a)
    {
        delete value;
        value = new int;
        *value = *a.value;
        return *this;
    }
    DIGI& operator=(DIGI&& a)
    {
        delete value;
        value = a.value;
        a.value = nullptr;
        return *this;
    }
    DIGI(DIGI&& a)
    {
        value = a.value;
        a.value = nullptr;
    }
    bool operator<(const DIGI& a)
    {
        return *value < *a.value;
    }
};

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
    srand(time(NULL));
    const size_t iters = rand() % TEST_MAX_ITERS;
    for(size_t i = 0; i < iters; i++)
    {
        size_t size = rand() % TEST_MAX_SIZE;
        if(size == 0)
            size = 1;
        for(size_t j = 0; j < 2; j++)
        {
            vec_digi a = vec_digi_init();
            std::vector<DIGI> b;
            // INIT DIRECTLY.
            if(j == 0)
            {
                vec_digi_resize(&a, size);
                b.resize(size);
            }
            // INIT WITH GROWTH.
            if(j == 1)
            {
                for(size_t i = 0; i < size; i++)
                {
                    const int value = rand() % INT_MAX;
                    vec_digi_push_back(&a, digi_init(value));
                    b.push_back(DIGI{value});
                }
            }
#define LIST X(CLEAR) X(ERASE) X(RESIZE) X(RESERVE) X(SHRINK_TO_FIT) X(SORT) X(COPY) X(SWAP) X(INSERT) X(ASSIGN) X(TOTAL)
#define X(name) name,
            enum { LIST };
#undef X
            size_t which = rand() % TOTAL;
#ifdef VERBOSE
#define X(name) #name,
            const char* names[] = { LIST };
#undef X
            printf("-> %lu : %s\n", j, names[which]);
#endif
            switch(which)
            {
                case CLEAR:
                {
                    b.clear();
                    vec_digi_clear(&a);
                    break;
                }
                case ERASE:
                {
                    const size_t index = rand() % a.size;
                    b.erase(b.begin() + index);
                    vec_digi_erase(&a, index);
                    break;
                }
                case INSERT:
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
                case RESIZE:
                {
                    const size_t resize = (size == 0) ? 0 : (rand() % (size * 3));
                    b.resize(resize);
                    vec_digi_resize(&a, resize);
                    break;
                }
                case RESERVE:
                {
                    const size_t capacity = (a.capacity == 0) ? 0 : (rand() % (a.capacity * 2));
                    b.reserve(capacity);
                    vec_digi_reserve(&a, capacity);
                    break;
                }
                case SHRINK_TO_FIT:
                {
                    b.shrink_to_fit();
                    vec_digi_shrink_to_fit(&a);
                    break;
                }
                case SORT:
                {
                    vec_digi_sort(&a, digi_compare);
                    std::sort(b.begin(), b.end());
                    break;
                }
                case COPY:
                {
                    vec_digi aa = vec_digi_copy(&a);
                    std::vector<DIGI> bb = b;
                    test_equal(&aa, bb);
                    vec_digi_free(&aa);
                    break;
                }
                case ASSIGN:
                {
                    const int value = rand() % INT_MAX;
                    size_t assign_size = rand() % a.size;
                    if(assign_size == 0)
                        assign_size = 1;
                    vec_digi_assign(&a, assign_size, digi_init(value));
                    b.assign(assign_size, DIGI{value});
                    break;
                }
                case SWAP:
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
    printf("%s: PASSED\n", __FILE__);
}
