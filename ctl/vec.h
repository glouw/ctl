#include <ctl.h>

#define CTL_A  CTL_TEMP(CTL_T, vec)
#define CTL_I  CTL_IMPL(CTL_A, it)

typedef struct
{
    CTL_T* value;
    CTL_T (*init_default)(void);
    void (*free)(CTL_T*);
    CTL_T (*copy)(CTL_T*);
    size_t size;
    size_t capacity;
}
CTL_A;

typedef struct CTL_I
{
    void (*step)(struct CTL_I*);
    CTL_T* ref;
    CTL_T* node;
    CTL_T* begin;
    CTL_T* end;
    CTL_T* next;
    size_t step_size;
    bool done;
}
CTL_I;

static inline CTL_A
CTL_IMPL(CTL_A, init)(void)
{
    static CTL_A zero;
    CTL_A self = zero;
#ifdef CTL_P
#undef CTL_P
#else
    self.init_default = CTL_IMPL(CTL_T, init_default);
    self.free = CTL_IMPL(CTL_T, free);
    self.copy = CTL_IMPL(CTL_T, copy);
#endif
    return self;
}

static inline bool
CTL_IMPL(CTL_A, empty)(CTL_A* self)
{
    return self->size == 0;
}

static inline CTL_T*
CTL_IMPL(CTL_A, at)(CTL_A* self, size_t index)
{
    return &self->value[index];
}

static inline CTL_T*
CTL_IMPL(CTL_A, front)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, at)(self, 0);
}

static inline CTL_T*
CTL_IMPL(CTL_A, back)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, at)(self, self->size - 1);
}

static inline CTL_T*
CTL_IMPL(CTL_A, begin)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, front)(self);
}

static inline CTL_T*
CTL_IMPL(CTL_A, end)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, back)(self) + 1;
}

static inline void
CTL_IMPL(CTL_A, set)(CTL_A* self, size_t index, CTL_T value)
{
    CTL_T* ref = CTL_IMPL(CTL_A, at)(self, index);
    if(self->free)
        self->free(ref);
    *ref = value;
}

static inline void
CTL_IMPL(CTL_A, pop_back)(CTL_A* self)
{
    static CTL_T zero;
    CTL_IMPL(CTL_A, set)(self, --self->size, zero);
}

static inline void
CTL_IMPL(CTL_A, wipe)(CTL_A* self, size_t n)
{
    while(n != 0)
    {
        CTL_IMPL(CTL_A, pop_back)(self);
        n -= 1;
    }
}

static inline void
CTL_IMPL(CTL_A, clear)(CTL_A* self)
{
    if(self->size > 0)
        CTL_IMPL(CTL_A, wipe)(self, self->size);
}

static inline void
CTL_IMPL(CTL_A, free)(CTL_A* self)
{
    static CTL_A zero;
    CTL_IMPL(CTL_A, clear)(self);
    free(self->value);
    *self = zero;
}

static inline void
CTL_IMPL(CTL_A, fit)(CTL_A* self, size_t capacity)
{
    static CTL_T zero;
    size_t overall = capacity;
    if(CTL_MUST_ALIGN_16(CTL_T))
        overall += 1;
    self->value = (CTL_T*) realloc(self->value, sizeof(CTL_T) * overall);
    if(CTL_MUST_ALIGN_16(CTL_T))
        for(size_t i = self->capacity; i < overall; i++)
            self->value[i] = zero;
    self->capacity = capacity;
}

static inline void
CTL_IMPL(CTL_A, reserve)(CTL_A* self, const size_t capacity)
{
    if(capacity != self->capacity)
    {
        size_t actual = 0;
        if(CTL_MUST_ALIGN_16(CTL_T))
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
            CTL_IMPL(CTL_A, fit)(self, actual);
    }
}

static inline void
CTL_IMPL(CTL_A, push_back)(CTL_A* self, CTL_T value)
{
    if(self->size == self->capacity)
        CTL_IMPL(CTL_A, reserve)(self, self->capacity == 0 ? 1 : 2 * self->capacity);
    *CTL_IMPL(CTL_A, at)(self, self->size) = value;
    self->size += 1;
}

static inline void
CTL_IMPL(CTL_A, resize)(CTL_A* self, size_t size)
{
    static CTL_T zero;
    if(size < self->size)
    {
        ptrdiff_t less = self->size - size;
        if(less > 0)
            CTL_IMPL(CTL_A, wipe)(self, less);
    }
    else
    {
        if(size > self->capacity)
        {
            size_t capacity = 2 * self->size;
            if(size > capacity)
                capacity = size;
            CTL_IMPL(CTL_A, reserve)(self, capacity);
        }
        while(self->size < size)
            CTL_IMPL(CTL_A, push_back)(self, self->init_default ? self->init_default() : zero);
    }
}

static inline void
CTL_IMPL(CTL_A, assign)(CTL_A* self, size_t size, CTL_T value)
{
    CTL_IMPL(CTL_A, resize)(self, size);
    for(size_t i = 0; i < size; i++)
        CTL_IMPL(CTL_A, set)(self, i, i == 0 ? value : self->copy ? self->copy(&value) : value);
}

static inline void
CTL_IMPL(CTL_A, shrink_to_fit)(CTL_A* self)
{
    CTL_IMPL(CTL_A, fit)(self, self->size);
}

static inline CTL_T*
CTL_IMPL(CTL_A, data)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, front)(self);
}

static inline void
CTL_IMPL(CTL_A, insert)(CTL_A* self, CTL_T* position, CTL_T value)
{
    if(self->size > 0)
    {
        size_t index = position - CTL_IMPL(CTL_A, begin)(self);
        CTL_IMPL(CTL_A, push_back)(self, *CTL_IMPL(CTL_A, back)(self));
        for(size_t i = self->size - 2; i > index; i--)
            self->value[i] = self->value[i - 1];
        self->value[index] = value;
    }
    else
        CTL_IMPL(CTL_A, push_back)(self, value);
}

static inline void
CTL_IMPL(CTL_A, erase)(CTL_A* self, CTL_T* position)
{
    static CTL_T zero;
    size_t index = position - CTL_IMPL(CTL_A, begin)(self);
    CTL_IMPL(CTL_A, set)(self, index, zero);
    for(size_t i = index; i < self->size - 1; i++)
    {
        self->value[i] = self->value[i + 1];
        self->value[i + 1] = zero;
    }
    self->size -= 1;
}

static inline void
CTL_IMPL(CTL_A, sort)(CTL_A* self, int (*compare)(CTL_T*, CTL_T*))
{
    if(self->size > 0)
        qsort(self->value, self->size, sizeof(CTL_T), (__compar_fn_t) compare); // ASSUME POINTER FUNCTION CAST IS SAFE.
}

static inline CTL_A
CTL_IMPL(CTL_A, copy)(CTL_A* self)
{
    CTL_A other = CTL_IMPL(CTL_A, init)();
    CTL_IMPL(CTL_A, reserve)(&other, self->size);
    while(other.size < self->size)
        CTL_IMPL(CTL_A, push_back)(&other, other.copy ? other.copy(&self->value[other.size]) : self->value[other.size]);
    return other;
}

static inline void
CTL_IMPL(CTL_A, swap)(CTL_A* self, CTL_A* other)
{
    CTL_A temp = *self;
    *self = *other;
    *other = temp;
}

static inline void
CTL_IMPL(CTL_I, step)(CTL_I* self)
{
    if(self->next < self->begin || self->next >= self->end)
        self->done = true;
    else
    {
        self->ref = self->node = self->next;
        self->next += self->step_size;
    }
}

static inline CTL_I
CTL_IMPL(CTL_I, by)(CTL_T* begin, CTL_T* end, size_t step_size)
{
    static CTL_I zero;
    CTL_I self = zero;
    if(begin == NULL || end == NULL)
        self.done = true;
    else
    {
        self.step = CTL_IMPL(CTL_I, step);
        self.begin = begin;
        self.end = end;
        self.next = begin + step_size;
        self.node = begin;
        self.ref = begin;
        self.step_size = step_size;
    }
    return self;
}

static inline CTL_I
CTL_IMPL(CTL_I, each)(CTL_A* a)
{
    return CTL_IMPL(CTL_A, empty)(a)
        ? CTL_IMPL(CTL_I, by)(NULL, NULL, 1)
        : CTL_IMPL(CTL_I, by)(CTL_IMPL(CTL_A, begin)(a), CTL_IMPL(CTL_A, end)(a), 1);
}

static inline void
CTL_IMPL(CTL_A, remove_if)(CTL_A* self, bool (*match)(CTL_T*))
{
    CTL_I it = CTL_IMPL(CTL_I, each)(self);
    CTL_FOR(it, {
        if(match(it.ref))
        {
            CTL_IMPL(CTL_A, erase)(self, it.node);
            it.end = CTL_IMPL(CTL_A, end)(self);
            it.next = it.node;
        }
    });
}

#undef CTL_T
#undef CTL_A
#undef CTL_I
