#include <_ctl_open.h>

#define A  TEMP(T, vec)
#define I  IMPL(A, it)
#define TZ IMPL(T, zero)
#define AZ IMPL(A, zero)
#define IZ IMPL(I, zero)
#define ALIGN16 (sizeof(T) == sizeof(char))

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
    size_t overall = capacity;
    if(ALIGN16)
        overall += 1;
    self->value = (T*) realloc(self->value, sizeof(T) * overall);
    if(ALIGN16)
        for(size_t i = self->capacity; i < overall; i++)
            self->value[i] = TZ;
    self->capacity = capacity;
}

static inline void
IMPL(A, reserve)(A* self, const size_t capacity)
{
    if(capacity != self->capacity)
    {
        size_t actual = 0;
        if(ALIGN16) // 16 BYTE BOUNDARY ALIGNMENT FOR STRINGS.
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
        if(capacity > self->capacity) // REGULAR ALIGNMENT.
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
            size_t capacity = 2 * self->size;
            if(size > capacity)
                capacity = size;
            IMPL(A, reserve)(self, capacity);
        }
        while(self->size < size)
            IMPL(A, push_back)(self, self->construct_default ? self->construct_default() : TZ);
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
    {
        self->value[i] = self->value[i + 1];
        self->value[i + 1] = TZ;
    }
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
        IMPL(A, push_back)(&other, other.copy
                ? other.copy(&self->value[other.size]) : self->value[other.size]);
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
        self->value = IMPL(A, at)(self->container, self->index = index);
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
#undef ALIGN16

#include <_ctl_close.h>
