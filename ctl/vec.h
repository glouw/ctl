#ifndef T
#error "Template type T undefined for <vec.h>"
#endif

#include <ctl.h>

#define A JOIN(vec, T)
#define I JOIN(A, it)

#define MUST_ALIGN_16(T) (sizeof(T) == sizeof(char))

typedef struct
{
    T* value;
    T (*init_default)(void);
    void (*free)(T*);
#ifdef COMPARE
    int (*compare)(T*, T*);
#endif
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
    int done;
}
I;

static inline T
JOIN(A, implicit_copy)(T* self)
{
    return *self;
}

static inline A
JOIN(A, init)(void)
{
    static A zero;
    A self = zero;
#ifdef P
#undef P
    self.copy = JOIN(A, implicit_copy);
#else
    self.init_default = JOIN(T, init_default);
    self.free = JOIN(T, free);
    self.copy = JOIN(T, copy);
#endif
    return self;
}

static inline A
JOIN(A, init_default)(void)
{
    return JOIN(A, init)();
}

static inline int
JOIN(A, empty)(A* self)
{
    return self->size == 0;
}

static inline T*
JOIN(A, at)(A* self, size_t index)
{
    return &self->value[index];
}

static inline T*
JOIN(A, front)(A* self)
{
    return JOIN(A, at)(self, 0);
}

static inline T*
JOIN(A, back)(A* self)
{
    return JOIN(A, at)(self, self->size - 1);
}

static inline T*
JOIN(A, begin)(A* self)
{
    return JOIN(A, front)(self);
}

static inline T*
JOIN(A, end)(A* self)
{
    return JOIN(A, back)(self) + 1;
}

static inline void
JOIN(A, set)(A* self, size_t index, T value)
{
    T* ref = JOIN(A, at)(self, index);
    if(self->free)
        self->free(ref);
    *ref = value;
}

static inline void
JOIN(A, pop_back)(A* self)
{
    static T zero;
    self->size -= 1;
    JOIN(A, set)(self, self->size, zero);
}

static inline void
JOIN(A, wipe)(A* self, size_t n)
{
    while(n != 0)
    {
        JOIN(A, pop_back)(self);
        n -= 1;
    }
}

static inline void
JOIN(A, clear)(A* self)
{
    if(self->size > 0)
        JOIN(A, wipe)(self, self->size);
}

static inline void
JOIN(A, free)(A* self)
{
    JOIN(A, clear)(self);
    free(self->value);
    *self = JOIN(A, init)();
}

static inline void
JOIN(A, fit)(A* self, size_t capacity)
{
    static T zero;
    size_t overall = capacity;
    if(MUST_ALIGN_16(T))
        overall += 1;
    self->value = (T*) realloc(self->value, overall * sizeof(T));
    if(MUST_ALIGN_16(T))
        for(size_t i = self->capacity; i < overall; i++)
            self->value[i] = zero;
    self->capacity = capacity;
}

static inline void
JOIN(A, reserve)(A* self, const size_t capacity)
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
            JOIN(A, fit)(self, actual);
    }
}

static inline void
JOIN(A, push_back)(A* self, T value)
{
    if(self->size == self->capacity)
        JOIN(A, reserve)(self, self->capacity == 0 ? 1 : 2 * self->capacity);
    *JOIN(A, at)(self, self->size) = value;
    self->size += 1;
}

static inline void
JOIN(A, resize)(A* self, size_t size)
{
    static T zero;
    if(size < self->size)
    {
        int64_t less = self->size - size;
        if(less > 0)
            JOIN(A, wipe)(self, less);
    }
    else
    {
        if(size > self->capacity)
        {
            size_t capacity = 2 * self->size;
            if(size > capacity)
                capacity = size;
            JOIN(A, reserve)(self, capacity);
        }
        while(self->size < size)
            JOIN(A, push_back)(self, self->init_default ? self->init_default() : zero);
    }
}

static inline void
JOIN(A, assign)(A* self, size_t size, T value)
{
    JOIN(A, resize)(self, size);
    for(size_t i = 0; i < size; i++)
        JOIN(A, set)(self, i, (i == 0) ? value : self->copy(&value));
}

static inline void
JOIN(A, shrink_to_fit)(A* self)
{
    JOIN(A, fit)(self, self->size);
}

static inline T*
JOIN(A, data)(A* self)
{
    return JOIN(A, front)(self);
}

static inline void
JOIN(A, insert)(A* self, T* position, T value)
{
    if(self->size > 0)
    {
        size_t index = position - JOIN(A, begin)(self);
        JOIN(A, push_back)(self, *JOIN(A, back)(self));
        for(size_t i = self->size - 2; i > index; i--)
            self->value[i] = self->value[i - 1];
        self->value[index] = value;
    }
    else
        JOIN(A, push_back)(self, value);
}

static inline void
JOIN(A, erase)(A* self, T* position)
{
    static T zero;
    size_t index = position - JOIN(A, begin)(self);
    JOIN(A, set)(self, index, zero);
    for(size_t i = index; i < self->size - 1; i++)
    {
        self->value[i] = self->value[i + 1];
        self->value[i + 1] = zero;
    }
    self->size -= 1;
}

static inline void
JOIN(A, ranged_sort)(A* self, int64_t a, int64_t b, int compare(T*, T*))
{
    if(a >= b)
        return;
    SWAP(T, &self->value[a], &self->value[(a + b) / 2]);
    int64_t z = a;
    for(int64_t i = a + 1; i <= b; i++)
        if(compare(&self->value[a], &self->value[i]))
        {
            z += 1;
            SWAP(T, &self->value[z], &self->value[i]);
        }
    SWAP(T, &self->value[a], &self->value[z]);
    JOIN(A, ranged_sort)(self, a, z - 1, compare);
    JOIN(A, ranged_sort)(self, z + 1, b, compare);
}

static inline void
JOIN(A, sort)(A* self, int compare(T*, T*))
{
    JOIN(A, ranged_sort)(self, 0, self->size - 1, compare);
}

static inline A
JOIN(A, copy)(A* self)
{
    A other = JOIN(A, init)();
#ifdef COMPARE
    other.compare = self->compare;
#endif
    JOIN(A, reserve)(&other, self->size);
    while(other.size < self->size)
        JOIN(A, push_back)(&other, other.copy(&self->value[other.size]));
    return other;
}

static inline void
JOIN(A, swap)(A* self, A* other)
{
    A temp = *self;
    *self = *other;
    *other = temp;
}

static inline void
JOIN(I, step)(I* self)
{
    if(self->next < self->begin || self->next >= self->end)
        self->done = 1;
    else
    {
        self->ref = self->next;
        self->next += 1;
    }
}

static inline I
JOIN(I, range)(T* begin, T* end)
{
    static I zero;
    I self = zero;
    if(begin && end)
    {
        self.step = JOIN(I, step);
        self.begin = begin;
        self.end = end;
        self.next = begin + 1;
        self.ref = begin;
    }
    else
        self.done = 1;
    return self;
}

static inline I
JOIN(I, each)(A* a)
{
    return JOIN(A, empty)(a)
        ? JOIN(I, range)(NULL, NULL)
        : JOIN(I, range)(JOIN(A, begin)(a), JOIN(A, end)(a));
}

static inline size_t
JOIN(A, remove_if)(A* self, int (*match)(T*))
{
    size_t erases = 0;
    foreach(A, self, it,
        if(match(it.ref))
        {
            JOIN(A, erase)(self, it.ref);
            it.end = JOIN(A, end)(self);
            it.next = it.ref;
            erases += 1;
        }
    );
    return erases;
}

static inline int
JOIN(A, equal)(A* self, A* other, int equal(T*, T*))
{
    if(self->size != other->size)
        return 0;
    I a = JOIN(I, each)(self);
    I b = JOIN(I, each)(other);
    while(!a.done && !b.done)
    {
        if(!equal(a.ref, b.ref))
            return 0;
        a.step(&a);
        b.step(&b);
    }
    return 1;
}

#ifdef COMPARE
#undef COMPARE
#endif

#undef A
#undef I
#undef MUST_ALIGN_16

// HOLD PRESERVES T IF OTHER CONTAINERS (EG. PQU.H) WISH TO EXTEND VEC.H
#ifdef HOLD
#undef HOLD
#else
#undef T
#endif
