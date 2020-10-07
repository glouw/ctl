#include <_ctl_open.h>

#define A  TEMP(T, vec)
#define I  IMPL(A, it)
#define TZ IMPL(T, zero)
#define AZ IMPL(A, zero)
#define IZ IMPL(I, zero)

typedef struct
{
    T (*construct_default)(void);
    void (*destruct)(T*);
    T (*copy)(T*);
    T* value;
    size_t size;
    size_t capacity;
}
A;

typedef struct I
{
    A* container;
    T* value;
    void (*step)(struct I*);
    size_t index;
    size_t start;
    size_t end;
    size_t step_size;
    bool done;
}
I;

static T TZ;

static A AZ;

static I IZ;

static inline A
IMPL(A, construct)(T (*construct_default)(void), void (*destruct)(T*), T (*copy)(T*))
{
    A self = AZ;
    self.construct_default = construct_default;
    self.destruct = destruct;
    self.copy = copy;
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

static inline void
IMPL(A, set)(A* self, size_t index, T value)
{
    T* ref = IMPL(A, at)(self, index);
    if(self->destruct)
        self->destruct(ref);
    *ref = value;
}

static inline void
IMPL(A, pop_back)(A* self)
{
    IMPL(A, set)(self, --self->size, TZ);
}

static inline void
IMPL(A, wipe)(A* self, size_t n)
{
    for(size_t i = 0; i < n; i++)
        IMPL(A, pop_back)(self);
}

static inline void
IMPL(A, clear)(A* self)
{
    IMPL(A, wipe)(self, self->size);
}

static inline void
IMPL(A, destruct)(A* self)
{
    IMPL(A, clear)(self);
    free(self->value);
    *self = AZ;
}

static inline void
IMPL(A, fit)(A* self, size_t capacity)
{
    self->capacity = capacity;
    self->value = (T*) realloc(self->value, sizeof(T) * capacity);
}

static inline void
IMPL(A, reserve)(A* self, size_t capacity)
{
    if(capacity > self->capacity)
        IMPL(A, fit)(self, capacity);
}

static inline void
IMPL(A, push_back)(A* self, T value)
{
    if(self->size == self->capacity)
    {
        size_t capacity = self->capacity == 0 ? 1 : 2 * self->capacity;
        IMPL(A, reserve)(self, capacity);
    }
    self->value[self->size++] = value;
}

static inline void
IMPL(A, resize)(A* self, size_t size)
{
    if(size < self->size)
        IMPL(A, wipe)(self, self->size - size);
    else
    {
        if(size > self->capacity)
        {
            size_t capacity = (self->size == 0) ? size : (self->size * 2);
            IMPL(A, reserve)(self, capacity);
        }
        while(self->size < size)
        {
            T value = self->construct_default ? self->construct_default() : TZ;
            IMPL(A, push_back)(self, value);
        }
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
IMPL(A, insert)(A* self, size_t index, T value)
{
    IMPL(A, push_back)(self, *IMPL(A, back)(self));
    for(size_t i = self->size - 2; i > index; i--)
        self->value[i] = self->value[i - 1];
    self->value[index] = value;
}

static inline void
IMPL(A, erase)(A* self, size_t index)
{
    IMPL(A, set)(self, index, TZ);
    for(size_t i = index; i < self->size - 1; i++)
        self->value[i] = self->value[i + 1];
    self->size -= 1;
}

static inline void
IMPL(A, sort)(A* self, int (*compare)(const void*, const void*))
{
    qsort(self->value, self->size, sizeof(T), compare);
}

static inline A
IMPL(A, copy)(A* self)
{
    A other = IMPL(A, construct)(self->construct_default, self->destruct, self->copy);
    IMPL(A, reserve)(&other, self->size);
    while(other.size < self->size)
    {
        T value = other.copy ? other.copy(&self->value[other.size]) : self->value[other.size];
        IMPL(A, push_back)(&other, value);
    }
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
IMPL(I, index)(I* self, size_t index)
{
    if(index >= self->start && index < self->end)
    {
        self->index = index;
        self->value = &self->container->value[index];
    }
    else
        self->done = true;
}

static void
IMPL(I, step)(I* self)
{
    IMPL(I, index)(self, self->index + self->step_size);
}

static inline I
IMPL(I, construct)(A* container, size_t start, size_t end, size_t step_size)
{
    I self = IZ;
    self.container = container;
    self.step = IMPL(I, step);
    self.start = start;
    self.end = end;
    self.step_size = step_size;
    IMPL(I, index)(&self, start);
    return self;
}

#undef A
#undef I
#undef TZ
#undef AZ
#undef IZ

#include <_ctl_close.h>
