#include <ctl/ctl.h>

#define A TEMPLATE(T, vec)

typedef struct
{
    T* value;
    void (*destruct)(T*);
    int begin;
    int end;
    int capacity;
}
A;

static A
IMPL(A, zero)(void)
{
    static A zero;
    return zero;
}

static A
IMPL(A, init)(int capacity, void (*destruct)(T*))
{
    const int min = 1;
    A self = IMPL(A, zero)();
    self.capacity = capacity < min ? min : capacity;
    self.value = (T*) malloc(self.capacity * sizeof(T));
    self.destruct = destruct;
    self.begin = self.end = self.capacity / 2;
    return self;
}

static void
IMPL(A, realloc)(A* self, int offset)
{
    T* value = (T*) malloc((self->capacity *= 2) * sizeof(T));
    int i;
    for(i = self->begin; i < self->end; i++)
        value[i + offset] = self->value[i];
    free(self->value);
    self->value = value;
    self->begin += offset;
    self->end += offset;
}

static void
IMPL(A, free)(A* self)
{
    if(self->destruct)
    {
        int i;
        for(i = self->begin; i < self->end; i++)
            self->destruct(&self->value[i]);
    }
    free(self->value);
    *self = IMPL(A, zero)();
}

static void
IMPL(A, push_back)(A* self, T value)
{
    if(self->end == self->capacity)
        IMPL(A, realloc)(self, 0);
    self->value[self->end++] = value;
}

static void
IMPL(A, push_front)(A* self, T value)
{
    if(self->begin == 0)
        IMPL(A, realloc)(self, self->capacity);
    self->value[--self->begin] = value;
}

static T
IMPL(A, peak_back)(A* self)
{
    return self->value[self->end - 1];
}

static T
IMPL(A, peak_front)(A* self)
{
    return self->value[self->begin];
}

static T
IMPL(A, pop_back)(A* self)
{
    T temp = IMPL(A, peak_back)(self);
    self->end -= 1;
    return temp;
}

static T
IMPL(A, pop_front)(A* self)
{
    T temp = IMPL(A, peak_front)(self);
    self->begin += 1;
    return temp;
}

static void
IMPL(A, del_back)(A* self)
{
    T temp = IMPL(A, pop_back)(self);
    if(self->destruct)
        self->destruct(&temp);
}

static void
IMPL(A, del_front)(A* self)
{
    T temp = IMPL(A, pop_front)(self);
    if(self->destruct)
        self->destruct(&temp);
}

static void
IMPL(A, swap)(A* self, int a, int b)
{
    T temp = self->value[a];
    self->value[a] = self->value[b];
    self->value[b] = temp;
}

static int
IMPL(A, weigh_back)(A* self, int index)
{
    return self->end - index < self->begin - index;
}

static void
IMPL(A, del)(A* self, int index)
{
    if(IMPL(A, weigh_back)(self, index))
    {
        IMPL(A, swap)(self, self->end - 1, index);
        IMPL(A, del_back)(self);
    }
    else
    {
        IMPL(A, swap)(self, self->begin, index);
        IMPL(A, del_front)(self);
    }
}

static void
IMPL(A, insert)(A* self, int index, T value)
{
    T temp = self->value[index];
    if(IMPL(A, weigh_back)(self, index))
        IMPL(A, push_back)(self, temp);
    else
        IMPL(A, push_front)(self, temp);
    self->value[index] = value;
}

static T*
IMPL(A, data)(A* self)
{
    return &self->value[self->begin];
}

static int
IMPL(A, size)(A* self)
{
    return self->end - self->begin;
}

static void
IMPL(A, sort)(A* self, int (*comparator)(const void* a, const void* b))
{
    qsort(IMPL(A, data)(self), IMPL(A, size)(self), sizeof(T), comparator);
}

#undef A
#undef T
