#ifndef __DIGI__H__
#define __DIGI__H__

#include <stdlib.h>
#include <functional>

typedef struct
{
    float* value;
}
digi;

static inline digi
digi_init(float value)
{
    digi self = {
        (float*) malloc(sizeof(*self.value))
    };
    *self.value = value;
    return self;
}

static inline void
digi_free(digi* self)
{
    free(self->value);
}

static inline int
digi_equal(digi* a, digi* b)
{
    return *a->value == *b->value;
}

static inline int
digi_compare(digi* a, digi* b)
{
    return *b->value < *a->value;
}

size_t
digi_hash(digi* d)
{
    return std::hash<float>{}(*d->value);
}

static inline digi
digi_copy(digi* self)
{
    digi copy = digi_init(0);
    *copy.value = *self->value;
    return copy;
}

static inline int
digi_is_odd(digi* d)
{
    return (int) *d->value % 2;
}

static inline int
digi_match(digi* a, digi* b)
{
    return *a->value == *b->value;
}

struct DIGI
{
    float* value;
    DIGI(float _value): value { new float {_value} }
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
        value = new float;
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
    bool operator<(const DIGI& a) const
    {
        return *value < *a.value;
    }
    bool operator==(const DIGI& a) const
    {
        return *value == *a.value;
    }
};

static inline bool
DIGI_is_odd(DIGI& d)
{
    return (int) *d.value % 2;
}

namespace std
{
    template<>
    struct hash<DIGI>
    {
        std::size_t operator()(const DIGI& d) const
        {
            return std::hash<float>{}(*d.value);
        }
    };
}

#endif
