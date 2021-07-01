//
// Vector
//

#ifndef T
#error "Template type T undefined for <vec.h>"
#endif

#include <ctl.h>

#define A JOIN(vec, T)
#define Z JOIN(A, it)

#define MUST_ALIGN_16(T) (sizeof(T) == sizeof(char))

typedef struct A
{
    T* value;
    void (*free)(T*);
#ifdef COMPARE
    int (*compare)(T*, T*);
#endif
    T (*copy)(T*);
    size_t size;
    size_t capacity;
}
A;

typedef struct Z
{
    void (*step)(struct Z*);
    T* ref;
    T* begin;
    T* end;
    T* next;
    int done;
}
Z;

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
JOIN(Z, step)(Z* self)
{
    if(self->next >= self->end)
        self->done = 1;
    else
    {
        self->ref = self->next;
        self->next += 1;
    }
}

static inline Z
JOIN(Z, range)(A* container, T* begin, T* end)
{
    (void) container;
    static Z zero;
    Z self = zero;
    if(begin && end)
    {
        self.step = JOIN(Z, step);
        self.begin = begin;
        self.end = end;
        self.next = begin + 1;
        self.ref = begin;
    }
    else
        self.done = 1;
    return self;
}

static inline int
JOIN(A, empty)(A* self)
{
    return self->size == 0;
}

static inline Z
JOIN(Z, each)(A* a)
{
    return JOIN(A, empty)(a)
         ? JOIN(Z, range)(a, NULL, NULL)
         : JOIN(Z, range)(a, JOIN(A, begin)(a), JOIN(A, end)(a));
}

static inline T
JOIN(A, implicit_copy)(T* self)
{
    return *self;
}

static inline int
JOIN(A, equal)(A* self, A* other, int _equal(T*, T*))
{
    if(self->size != other->size)
        return 0;
    Z a = JOIN(Z, each)(self);
    Z b = JOIN(Z, each)(other);
    while(!a.done && !b.done)
    {
        if(!_equal(a.ref, b.ref))
            return 0;
        a.step(&a);
        b.step(&b);
    }
    return 1;
}

static inline void
JOIN(A, swap)(A* self, A* other)
{
    A temp = *self;
    *self = *other;
    *other = temp;
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
    self.free = JOIN(T, free);
    self.copy = JOIN(T, copy);
#endif
    return self;
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
JOIN(A, resize)(A* self, size_t size, T value)
{
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
        for(size_t i = 0; self->size < size; i++)
            JOIN(A, push_back)(self, self->copy(&value));
    }
    if(self->free)
        self->free(&value);
}

static inline void
JOIN(A, assign)(A* self, size_t size, T value)
{
    JOIN(A, resize)(self, size, self->copy(&value));
    for(size_t i = 0; i < size; i++)
        JOIN(A, set)(self, i, self->copy(&value));
    if(self->free)
        self->free(&value);
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
JOIN(A, insert)(A* self, size_t index, T value)
{
    if(self->size > 0)
    {
        JOIN(A, push_back)(self, *JOIN(A, back)(self));
        for(size_t i = self->size - 2; i > index; i--)
            self->value[i] = self->value[i - 1];
        self->value[index] = value;
    }
    else
        JOIN(A, push_back)(self, value);
}

static inline void
JOIN(A, erase)(A* self, size_t index)
{
    static T zero;
    JOIN(A, set)(self, index, zero);
    for(size_t i = index; i < self->size - 1; i++)
    {
        self->value[i] = self->value[i + 1];
        self->value[i + 1] = zero;
    }
    self->size -= 1;
}

static inline void
JOIN(A, ranged_sort)(A* self, int64_t a, int64_t b, int _compare(T*, T*))
{
    if(a >= b)
        return;
    int64_t mid = (a + b) / 2;
    SWAP(T, &self->value[a], &self->value[mid]);
    int64_t z = a;
    for(int64_t i = a + 1; i <= b; i++)
        if(_compare(&self->value[a], &self->value[i]))
        {
            z += 1;
            SWAP(T, &self->value[z], &self->value[i]);
        }
    SWAP(T, &self->value[a], &self->value[z]);
    JOIN(A, ranged_sort)(self, a, z - 1, _compare);
    JOIN(A, ranged_sort)(self, z + 1, b, _compare);
}

static inline void
JOIN(A, sort)(A* self, int _compare(T*, T*))
{
    JOIN(A, ranged_sort)(self, 0, self->size - 1, _compare);
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

static inline size_t
JOIN(A, remove_if)(A* self, int _match(T*))
{
    size_t erases = 0;
    foreach(A, self, it)
    {
        if(_match(it.ref))
        {
            size_t index = it.ref - JOIN(A, begin)(self);
            JOIN(A, erase)(self, index);
            it.end = JOIN(A, end)(self);
            it.next = it.ref;
            erases += 1;
        }
    }
    return erases;
}

static inline T*
JOIN(A, find)(A* self, T key, int _equal(T*, T*))
{
    foreach(A, self, it)
        if(_equal(it.ref, &key))
            return it.ref;
    return NULL;
}

#ifdef COMPARE
#undef COMPARE
#endif

#undef A
#undef Z
#undef MUST_ALIGN_16

// Hold preserves `T` if other containers
// (eg. `pqu.h`) wish to extend `vec.h`.
#ifdef HOLD
#undef HOLD
#else
#undef T
#endif
