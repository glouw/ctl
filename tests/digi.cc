#pragma once

// THESE DIGI STRUCTS BEHAVE IDENTICALLY AND ARE USED AS THE BASIS
// FOR TESTING COPY / FREE / CONSTRUCT FOR STL AND CTL CONTAINERS.

typedef struct
{
    int* value;
}
digi;

static inline digi
digi_init(int value)
{
    digi self = {
        (int*) malloc(sizeof(*self.value))
    };
    *self.value = value;
    return self;
}

static inline digi
digi_init_default(void)
{
    return digi_init(0);
}

static inline void
digi_free(digi* self)
{
    free(self->value);
}

static inline int
digi_compare(digi* a, digi* b)
{
    return *b->value < *a->value;
}

static inline digi
digi_copy(digi* self)
{
    digi copy = digi_init_default();
    *copy.value = *self->value;
    return copy;
}

struct DIGI
{
    int* value;

    DIGI(int _value): value { new int {_value} }
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
    bool operator<(const DIGI& a) const
    {
        return *value < *a.value;
    }
    bool operator==(const DIGI& a) const
    {
        return *value == *a.value;
    }
};
