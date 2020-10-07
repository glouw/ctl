#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <assert.h>

typedef struct
{
    int* value;
}
digi;

static digi
digi_construct(int value)
{
    digi self = {
        (int*) malloc(sizeof(*self.value))
    };
    *self.value = value;
    return self;
}

static digi
digi_construct_default(void)
{
    return digi_construct(0);
}

static void
digi_destruct(digi* self)
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
    digi copy = digi_construct_default();
    *copy.value = *self->value;
    return copy;
}

#define T digi
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
        int index = 0;
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
#define MAX_VALUE (INT_MAX)
#define MAX_SIZE     (8192)
#define MAX_ITERS    (8192)
    srand(time(NULL));
    const int size = rand() % MAX_SIZE;
    const int iters = rand() % MAX_ITERS;
    for(int i = 0; i < iters; i++)
    {
        vec_digi a = vec_digi_construct(digi_construct_default, digi_destruct, digi_copy);
        std::vector<DIGI> b;
        for(int i = 0; i < size; i++)
        {
            const int value = rand() % MAX_VALUE;
            vec_digi_push_back(&a, digi_construct(value));
            b.push_back(DIGI{value});
        }
#define LIST X(CLEAR) X(ERASE) X(RESIZE) X(CAPACITY) X(SHRINK_TO_FIT) X(SORT) X(COPY) X(SWAP) X(INSERT) X(ASSIGN) X(TOTAL)
#define X(name) name,
        enum { LIST };
#undef X
#define X(name) #name,
        const char* names[] = { LIST };
#undef X
        size_t which = rand() % TOTAL;
        puts(names[which]);
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
                const int value = rand() % MAX_VALUE;
                const size_t index = rand() % a.size;
                b.insert(b.begin() + index, DIGI{value});
                vec_digi_insert(&a, index, digi_construct(value));
                break;
            }
        case RESIZE:
            {
                const size_t resize = (size == 0) ? 0 : (rand() % (size * 2));
                b.resize(resize);
                vec_digi_resize(&a, resize);
                break;
            }
        case CAPACITY:
            {
                const size_t capacity = (b.capacity() == 0) ? 0 : (rand() % (b.capacity() * 2));
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
                vec_digi_destruct(&aa);
                break;
            }
        case ASSIGN:
            {
                const int value = rand() % MAX_VALUE;
                const size_t size = rand() % a.size + 1;
                vec_digi_assign(&a, size, digi_construct(value));
                b.assign(size, DIGI{value});
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
                vec_digi_destruct(&aaa);
                break;
            }
        }
        test_equal(&a, b);
        vec_digi_destruct(&a);
    }
    printf("%s: PASSED\n", __FILE__);
}
