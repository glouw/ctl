#include <ctl.h>

#define A TEMP(T, vec)
#define I IMPL(A, it)

typedef struct
{
    T* value;
    T (*init_default)(void);
    void (*free)(T*);
    T (*copy)(T*);
    size_t size;
    size_t capacity;
}
A;

typedef struct I
{
    void (*step)(struct I*);
    T* ref;
    T* begin;
    T* end;
    T* next;
    bool done;
}
I;

static inline A
IMPL(A, init)(void)
{
    static A zero;
    A self = zero;
#ifdef P
#undef P
#else
    self.init_default = IMPL(T, init_default);
    self.free = IMPL(T, free);
    self.copy = IMPL(T, copy);
#endif
    return self;
}

static inline bool
IMPL(A, empty)(A* self)
{
    return self->size == 0;
}

static inline T*
IMPL(A, at)(A* self, size_t index)
{
    return &self->value[index];
}

static inline T*
IMPL(A, front)(A* self)
{
    return IMPL(A, at)(self, 0);
}

static inline T*
IMPL(A, back)(A* self)
{
    return IMPL(A, at)(self, self->size - 1);
}

static inline T*
IMPL(A, begin)(A* self)
{
    return IMPL(A, front)(self);
}

static inline T*
IMPL(A, end)(A* self)
{
    return IMPL(A, back)(self) + 1;
}

static inline void
IMPL(A, set)(A* self, size_t index, T value)
{
    T* ref = IMPL(A, at)(self, index);
    if(self->free)
        self->free(ref);
    *ref = value;
}

static inline void
IMPL(A, pop_back)(A* self)
{
    static T zero;
    IMPL(A, set)(self, --self->size, zero);
}

static inline void
IMPL(A, wipe)(A* self, size_t n)
{
    while(n != 0)
    {
        IMPL(A, pop_back)(self);
        n -= 1;
    }
}

static inline void
IMPL(A, clear)(A* self)
{
    if(self->size > 0)
        IMPL(A, wipe)(self, self->size);
}

static inline void
IMPL(A, free)(A* self)
{
    static A zero;
    IMPL(A, clear)(self);
    free(self->value);
    *self = zero;
}

static inline void
IMPL(A, fit)(A* self, size_t capacity)
{
    static T zero;
    size_t overall = capacity;
    if(MUST_ALIGN_16(T))
        overall += 1;
    self->value = (T*) realloc(self->value, sizeof(T) * overall);
    if(MUST_ALIGN_16(T))
        for(size_t i = self->capacity; i < overall; i++)
            self->value[i] = zero;
    self->capacity = capacity;
}

static inline void
IMPL(A, reserve)(A* self, const size_t capacity)
{
    if(capacity != self->capacity)
    {
        size_t actual = 0;
        if(MUST_ALIGN_16(T))
        {
            if(capacity <= self->size)
                actual = self->size;
            else
            if(capacity > self->size && capacity < self->capacity)
                actual = capacity;
            else
            {
                actual = 2 * self->capacity;
                if(capacity > actual)
                    actual = capacity;
            }
        }
        else
        if(capacity > self->capacity)
            actual = capacity;
        if(actual > 0)
            IMPL(A, fit)(self, actual);
    }
}

static inline void
IMPL(A, push_back)(A* self, T value)
{
    if(self->size == self->capacity)
        IMPL(A, reserve)(self, self->capacity == 0 ? 1 : 2 * self->capacity);
    *IMPL(A, at)(self, self->size) = value;
    self->size += 1;
}

static inline void
IMPL(A, resize)(A* self, size_t size)
{
    static T zero;
    if(size < self->size)
    {
        ptrdiff_t less = self->size - size;
        if(less > 0)
            IMPL(A, wipe)(self, less);
    }
    else
    {
        if(size > self->capacity)
        {
            size_t capacity = 2 * self->size;
            if(size > capacity)
                capacity = size;
            IMPL(A, reserve)(self, capacity);
        }
        while(self->size < size)
            IMPL(A, push_back)(self, self->init_default ? self->init_default() : zero);
    }
}

static inline void
IMPL(A, assign)(A* self, size_t size, T value)
{
    IMPL(A, resize)(self, size);
    for(size_t i = 0; i < size; i++)
        IMPL(A, set)(self, i, i == 0 ? value : self->copy ? self->copy(&value) : value);
}

static inline void
IMPL(A, shrink_to_fit)(A* self)
{
    IMPL(A, fit)(self, self->size);
}

static inline T*
IMPL(A, data)(A* self)
{
    return IMPL(A, front)(self);
}

static inline void
IMPL(A, insert)(A* self, T* position, T value)
{
    if(self->size > 0)
    {
        size_t index = position - IMPL(A, begin)(self);
        IMPL(A, push_back)(self, *IMPL(A, back)(self));
        for(size_t i = self->size - 2; i > index; i--)
            self->value[i] = self->value[i - 1];
        self->value[index] = value;
    }
    else
        IMPL(A, push_back)(self, value);
}

static inline void
IMPL(A, erase)(A* self, T* position)
{
    static T zero;
    size_t index = position - IMPL(A, begin)(self);
    IMPL(A, set)(self, index, zero);
    for(size_t i = index; i < self->size - 1; i++)
    {
        self->value[i] = self->value[i + 1];
        self->value[i + 1] = zero;
    }
    self->size -= 1;
}

static inline void
IMPL(A, sort)(A* self, int compare(T*, T*))
{
    if(self->size > 0)
        qsort(self->value, self->size, sizeof(T), (__compar_fn_t) compare); // ASSUME POINTER FUNCTION CAST IS SAFE.
}

static inline A
IMPL(A, copy)(A* self)
{
    A other = IMPL(A, init)();
    IMPL(A, reserve)(&other, self->size);
    while(other.size < self->size)
        IMPL(A, push_back)(&other, other.copy ? other.copy(&self->value[other.size]) : self->value[other.size]);
    return other;
}

static inline void
IMPL(A, swap)(A* self, A* other)
{
    A temp = *self;
    *self = *other;
    *other = temp;
}

static inline void
IMPL(I, step)(I* self)
{
    if(self->next < self->begin || self->next >= self->end)
        self->done = true;
    else
    {
        self->ref = self->next;
        self->next += 1;
    }
}

static inline I
IMPL(I, range)(T* begin, T* end)
{
    static I zero;
    I self = zero;
    if(begin && end)
    {
        self.step = IMPL(I, step);
        self.begin = begin;
        self.end = end;
        self.next = begin + 1;
        self.ref = begin;
    }
    else
        self.done = true;
    return self;
}

static inline I
IMPL(I, each)(A* a)
{
    return IMPL(A, empty)(a)
        ? IMPL(I, range)(NULL, NULL)
        : IMPL(I, range)(IMPL(A, begin)(a), IMPL(A, end)(a));
}

static inline void
IMPL(A, remove_if)(A* self, bool (*match)(T*))
{
    foreach(A, self, it,
        if(match(it.ref))
        {
            IMPL(A, erase)(self, it.ref);
            it.end = IMPL(A, end)(self);
            it.next = it.ref;
        }
    );
}

#undef T
#undef A
#undef I
