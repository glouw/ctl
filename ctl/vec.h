#include <ctl/ctl.h>

#define C TEMPLATE(T, vec)

typedef struct
{
    T* values;
    void (*destruct)(T*);
    int begin;
    int end;
    int capacity;
}
C;

static C
IMPL(C, zero)(void)
{
    static C zero;
    return zero;
}

static C
IMPL(C, init)(int capacity, void (*destruct)(T*))
{
    C self = IMPL(C, zero)();
    self.values = malloc(capacity * sizeof(T));
    self.destruct = destruct;
    self.begin = self.end = capacity / 2;
    self.capacity = capacity;
    return self;
}

static void
IMPL(C, realloc)(C* self, int offset)
{
    T* values = malloc((self->capacity *= 2) * sizeof(T));
    int i;
    for(i = self->begin; i < self->end; i++)
        values[i + offset] = self->values[i];
    free(self->values);
    self->values = values;
    self->begin += offset;
    self->end += offset;
}

static void
IMPL(C, free)(C* self)
{
    if(self->destruct)
    {
        int i;
        for(i = self->begin; i < self->end; i++)
            self->destruct(&self->values[i]);
    }
    free(self->values);
    *self = IMPL(C, zero)();
}

static void
IMPL(C, push_back)(C* self, T value)
{
    if(self->end == self->capacity)
        IMPL(C, realloc)(self, 0);
    self->values[self->end++] = value;
}

static void
IMPL(C, push_front)(C* self, T value)
{
    if(self->begin == 0)
        IMPL(C, realloc)(self, self->capacity);
    self->values[--self->begin] = value;
}

static T
IMPL(C, peak_back)(C* self)
{
    return self->values[self->end - 1];
}

static T
IMPL(C, peak_front)(C* self)
{
    return self->values[self->begin];
}

static T
IMPL(C, pop_back)(C* self)
{
    T temp = IMPL(C, peak_back)(self);
    self->end -= 1;
    return temp;
}

static T
IMPL(C, pop_front)(C* self)
{
    T temp = IMPL(C, peak_front)(self);
    self->begin += 1;
    return temp;
}

static void
IMPL(C, del_back)(C* self)
{
    T temp = IMPL(C, pop_back)(self);
    if(self->destruct)
        self->destruct(&temp);
}

static void
IMPL(C, del_front)(C* self)
{
    T temp = IMPL(C, pop_front)(self);
    if(self->destruct)
        self->destruct(&temp);
}

static void
IMPL(C, swap)(C* self, int a, int b)
{
    T temp = self->values[a];
    self->values[a] = self->values[b];
    self->values[b] = temp;
}

static void
IMPL(C, del)(C* self, int index)
{
    int look = index + self->begin;
    if(self->end - look < index)
    {
        IMPL(C, swap)(self, self->end - 1, look);
        IMPL(C, del_back)(self);
    }
    else
    {
        IMPL(C, swap)(self, self->begin, look);
        IMPL(C, del_front)(self);
    }
}

static T*
IMPL(C, data)(C* self)
{
    return &self->values[self->begin];
}

static int
IMPL(C, size)(C* self)
{
    return self->end - self->begin;
}

static void
IMPL(C, sort)(C* self, int (*comparator)(const void* a, const void* b))
{
    qsort(IMPL(C, data)(self), IMPL(C, size)(self), sizeof(T), comparator);
}

#undef C
#undef T
