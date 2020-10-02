#include <ctl.h>

#define A  CTL_TEMP(T, vec)
#define I  CTL_IMPL(A, it)
#define AZ CTL_IMPL(A, zero)
#define IZ CTL_IMPL(I, zero)

typedef struct
{
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
    int done;
}
I;

static A AZ;

static I IZ;

static inline A
CTL_IMPL(A, construct)(void (*destruct)(T*), T (*copy)(T*))
{
    A self = AZ;
    self.destruct = destruct;
    self.copy = copy;
    return self;
}

static inline T*
CTL_IMPL(A, at)(A* self, size_t index)
{
    return &self->value[index];
}

static inline T*
CTL_IMPL(A, front)(A* self)
{
    return CTL_IMPL(A, at)(self, 0);
}

static inline T*
CTL_IMPL(A, back)(A* self)
{
    return CTL_IMPL(A, at)(self, self->size - 1);
}

static inline void
CTL_IMPL(A, pop_back)(A* self)
{
    T* back = CTL_IMPL(A, back)(self);
    if(self->destruct)
        self->destruct(back);
    static T zero;
    *back = zero;
    self->size -= 1;
}

static inline void
CTL_IMPL(A, _clear)(A* self, size_t n)
{
    for(size_t i = 0; i < n; i++)
        CTL_IMPL(A, pop_back)(self);
}

static inline void
CTL_IMPL(A, clear)(A* self)
{
    CTL_IMPL(A, _clear)(self, self->size);
}

static inline void
CTL_IMPL(A, destruct)(A* self)
{
    CTL_IMPL(A, clear)(self);
    free(self->value);
    *self = AZ;
}

static inline void
CTL_IMPL(A, reserve)(A* self, size_t capacity)
{
    if(capacity > self->capacity)
    {
        self->capacity = capacity;
        self->value = (T*) realloc(self->value, sizeof(T) * capacity);
        static T zero;
        for(size_t i = self->size; i < self->capacity; i++)
            self->value[i] = zero;
    }
}

static inline void
CTL_IMPL(A, resize)(A* self, size_t size)
{
    if(size < self->size)
        CTL_IMPL(A, _clear)(self, self->size - size);
    else
    {
        CTL_IMPL(A, reserve)(self, size);
        size_t diff = size - self->size;
        static T zero;
        for(size_t i = self->size; i < diff; i++)
            self->value[i] = zero;
        self->size = size;
    }
}

static inline int
CTL_IMPL(A, empty)(A* self)
{
    return self->size == 0;
}

static inline void
CTL_IMPL(A, shrink_to_fit)(A* self)
{
    self->capacity = self->size;
    self->value = (T*) realloc(self->value, sizeof(T) * self->capacity);
}

static inline T*
CTL_IMPL(A, data)(A* self)
{
    return CTL_IMPL(A, front)(self);
}

static inline void
CTL_IMPL(A, set)(A* self, size_t index, T value)
{
    T* ref = CTL_IMPL(A, at)(self, index);
    if(self->destruct)
        self->destruct(ref);
    *ref = value;
}

static inline void
CTL_IMPL(A, push_back)(A* self, T value)
{
    if(self->size == self->capacity)
    {
        size_t capacity = self->capacity == 0 ? 1 : 2 * self->capacity;
        CTL_IMPL(A, reserve)(self, capacity);
    }
    self->value[self->size++] = value;
}

static inline void
CTL_IMPL(A, insert)(A* self, size_t index, T value)
{
    CTL_IMPL(A, push_back)(self, *CTL_IMPL(A, back)(self));
    for(size_t i = self->size - 2; i > index; i--)
        self->value[i] = self->value[i - 1];
    self->value[index] = value;
}

static inline void
CTL_IMPL(A, erase)(A* self, size_t index)
{
    if(self->destruct)
        self->destruct(&self->value[index]);
    for(size_t i = index; i < self->size - 1; i++)
        self->value[i] = self->value[i + 1];
    self->size -= 1;
}

static inline A
CTL_IMPL(A, copy)(A* self)
{
    A other = CTL_IMPL(A, construct)(self->destruct, self->copy);
    CTL_IMPL(A, resize)(&other, self->size);
    for(size_t i = 0; i < other.size; i++)
        other.value[i] = other.copy
            ? other.copy(&self->value[i])
            : self->value[i];
    return other;
}

static inline A
CTL_IMPL(A, move)(A* self)
{
    A temp = *self;
    *self = AZ;
    return temp;
}

static inline void
CTL_IMPL(A, swap)(A* self, A* other)
{
    A moved = CTL_IMPL(A, move)(other);
    *other = CTL_IMPL(A, move)(self);
    *self = CTL_IMPL(A, move)(&moved);
}

static inline void
CTL_IMPL(I, index)(I* self, size_t index)
{
    if(index >= self->start && index < self->end)
    {
        self->index = index;
        self->value = &self->container->value[index];
    }
    else
        self->done = 1;
}

static void
CTL_IMPL(I, step)(I* self)
{
    CTL_IMPL(I, index)(self, self->index + self->step_size);
}

static inline I
CTL_IMPL(I, construct)(A* container, size_t start, size_t end, size_t step_size)
{
    I self = IZ;
    self.container = container;
    self.step = CTL_IMPL(I, step);
    self.start = start;
    self.end = end;
    self.step_size = step_size;
    CTL_IMPL(I, index)(&self, start);
    return self;
}

#undef T
#undef A
#undef I
#undef Z
