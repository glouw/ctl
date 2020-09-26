#define C TEMPLATE(vec)

typedef struct
{
    T* values;
    void (*destruct)(T*);
    int begin;
    int end;
    int capacity;
}
C;

DECL C
IMPL(_zero)(void)
{
    static C zero;
    return zero;
}

DECL C
IMPL(_init)(int capacity, void (*destruct)(T*))
{
    C self = IMPL(_zero)();
    self.values = malloc(capacity * sizeof(T));
    self.destruct = destruct;
    self.begin = self.end = capacity / 2;
    self.capacity = capacity;
    return self;
}

DECL void
IMPL(_realloc)(C* self, int offset)
{
    self->capacity *= 2;
    T* values = malloc(self->capacity * sizeof(T));
    for(int i = self->begin; i < self->end; i++)
        values[i + offset] = self->values[i];
    free(self->values);
    self->values = values;
    self->begin += offset;
    self->end += offset;
}

DECL void
IMPL(_free)(C* self)
{
    if(self->destruct)
        for(int i = self->begin; i < self->end; i++)
            self->destruct(&self->values[i]);
    free(self->values);
    *self = IMPL(_zero)();
}

DECL void
IMPL(_push_back)(C* self, T value)
{
    if(self->end == self->capacity)
        IMPL(_realloc)(self, 0);
    self->values[self->end++] = value;
}

DECL void
IMPL(_push_front)(C* self, T value)
{
    if(self->begin == 0)
        IMPL(_realloc)(self, self->capacity);
    self->values[--self->begin] = value;
}

DECL T
IMPL(_peak_back)(C* self)
{
    return self->values[self->end - 1];
}

DECL T
IMPL(_peak_front)(C* self)
{
    return self->values[self->begin];
}

DECL T
IMPL(_pop_back)(C* self)
{
    T t = IMPL(_peak_back)(self);
    self->end -= 1;
    return t;
}

DECL T
IMPL(_pop_front)(C* self)
{
    T t = IMPL(_peak_front)(self);
    self->begin += 1;
    return t;
}

DECL void
IMPL(_delete_back)(C* self)
{
    T t = IMPL(_pop_back)(self);
    if(self->destruct)
        self->destruct(&t);
}

DECL void
IMPL(_delete_front)(C* self)
{
    T t = IMPL(_pop_front)(self);
    if(self->destruct)
        self->destruct(&t);
}

DECL T*
IMPL(_data)(C* self)
{
    return &self->values[self->begin];
}

DECL int
IMPL(_size)(C* self)
{
    return self->end - self->begin;
}

DECL void
IMPL(_sort)(C* self, int (*comparator)(const void* a, const void* b))
{
    qsort(IMPL(_data)(self), IMPL(_size)(self), sizeof(T), comparator);
}

#undef C
#undef T
