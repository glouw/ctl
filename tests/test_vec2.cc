#include <stdio.h>
#include <stdlib.h>
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
    digi self = { (int*) malloc(sizeof(*self.value)) };
    *self.value = value;
    return self;
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
    return *aa->value < *bb->value;
}

static digi
digi_copy(digi* self)
{
    digi copy = digi_construct(0);
    if(copy.value != NULL)
        *copy.value = *self->value;
    return copy;
}

#define T digi
#include <vec.h>

struct DIGI
{
    int* value;
    DIGI(int value): value{new int{value}}
    {
    }
    DIGI()
    {
        value = nullptr;
    }
    ~DIGI()
    {
        delete value;
        value = nullptr;
    }
    DIGI(const DIGI& other): DIGI{0}
    {
        if(other.value != nullptr)
            *value = *other.value;
    }
    bool operator==(const DIGI& other)
    {
        return *value == *other.value;
    }
    void operator=(const DIGI& other)
    {
        *value = *other.value;
    }
};

bool DIGI_compare(const DIGI& a, const DIGI& b)
{
    return *b.value < *a.value;
}

static void
test_equal(vec_digi* a, std::vector<DIGI>& b)
{
    int index = 0;
    for(auto& d : b)
    {
        digi* ref = vec_digi_at(a, index);
        if(d.value != nullptr && ref->value != NULL)
            assert(*d.value == *ref->value);
        index += 1;
    }
    assert(a->capacity == b.capacity());
    assert(a->size == b.size());
    assert(vec_digi_empty(a) == b.empty());
    if(a->size > 0)
    {
        DIGI& bf = b.front();
        DIGI& bb = b.back();
        digi* af = vec_digi_front(a);
        digi* ab = vec_digi_back(a);
        if(bf.value != nullptr && af->value != NULL) assert(*af->value == *bf.value);
        if(bb.value != nullptr && ab->value != NULL) assert(*ab->value == *bb.value);
    }
}

int
main(void)
{
    srand(time(NULL));
    const int size = rand() % 8192;
    const int iters = rand() % 1024;
    for(int i = 0; i < iters; i++)
    {
        vec_digi a = vec_digi_construct(digi_destruct, digi_copy);
        std::vector<DIGI> b;
        for(int i = 0; i < size; i++)
        {
            int value = rand() % 512;
            vec_digi_push_back(&a, digi_construct(value));
            b.push_back(DIGI{value});
        }
        if(rand() % 100 == 0)
        {
            puts("clear");
            b.clear();
            vec_digi_clear(&a);
        }
        if(rand() % 2)
        {
            if(rand() % 2)
            {
                const int resize = (size == 0) ? (rand() % 10) : (rand() % (size * 2));
                b.resize(resize);
                vec_digi_resize(&a, resize);
            }
            if(rand() % 2)
            {
                const int capacity = (a.capacity == 0) ? (rand() % 10) : (rand() % (a.capacity * 2));
                b.reserve(capacity);
                vec_digi_reserve(&a, capacity);
            }
        }
        else
        {
            if(rand() % 2)
            {
                vec_digi_sort(&a, digi_compare);
                std::sort(b.begin(), b.end(), DIGI_compare);
            }
        }
        test_equal(&a, b);
        vec_digi_destruct(&a);
    }
    printf("%s: PASSED\n", __FILE__);
}
