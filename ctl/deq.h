#include <ctl.h>

#define A TEMP(T, deq)
#define B IMPL(A, bucket)
#define I IMPL(A, it)

#define DEQ_BUCKET_SIZE (64)

#define SWAP(self, a, b) { T temp = *IMPL(A, at)(self, a); *IMPL(A, at)(self, a) = *IMPL(A, at)(self, b); *IMPL(A, at)(self, b) = temp; }

typedef struct B
{
    T value[DEQ_BUCKET_SIZE];
    int16_t a;
    int16_t b;
}
B;

typedef struct
{
    T (*init_default)(void);
    void (*free)(T*);
    T (*copy)(T*);
    B** pages;
    size_t mark_a;
    size_t mark_b;
    size_t capacity;
    size_t size;
}
A;

typedef struct I
{
    void (*step)(struct I*);
    A* container;
    T* ref;
    size_t index;
    size_t index_next;
    size_t index_last;
    int done;
}
I;

static inline int
IMPL(A, empty)(A* self)
{
    return self->size == 0;
}

static inline T
IMPL(A, __implicit_copy)(T* self)
{
    return *self;
}

static inline A
IMPL(A, init)(void)
{
    static A zero;
    A self = zero;
#ifdef P
#undef P
    self.copy = IMPL(A, __implicit_copy);
#else
    self.init_default = IMPL(T, init_default);
    self.free = IMPL(T, free);
    self.copy = IMPL(T, copy);
#endif
    return self;
}

static inline A
IMPL(A, init_default)(void)
{
    return IMPL(A, init)();
}

static inline B*
IMPL(B, init)(size_t cut)
{
    B* self = (B*) malloc(sizeof(B));
    self->a = self->b = cut;
    return self;
}

static inline B**
IMPL(A, first)(A* self)
{
    return &self->pages[self->mark_a];
}

static inline B**
IMPL(A, last)(A* self)
{
    return &self->pages[self->mark_b - 1];
}

static inline T*
IMPL(A, at)(A* self, size_t index)
{
    if(self->size == 0)
        return NULL;
    else
    {
        B* first = *IMPL(A, first)(self);
        size_t actual = index + first->a;
        B* page = self->pages[self->mark_a + actual / DEQ_BUCKET_SIZE];
        return &page->value[actual % DEQ_BUCKET_SIZE];
    }
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
IMPL(A, alloc)(A* self, size_t capacity, size_t shift_from)
{
    self->capacity = capacity;
    self->pages = (B**) realloc(self->pages, capacity * sizeof(B*));
    size_t shift = (self->capacity - shift_from) / 2;
    size_t i = self->mark_b;
    while(i != 0)
    {
        i -= 1;
        self->pages[i + shift] = self->pages[i];
    }
    self->mark_a += shift;
    self->mark_b += shift;
}

static inline void
IMPL(A, push_front)(A* self, T value)
{
    if(self->capacity == 0)
    {
        IMPL(A, alloc)(self, 1, 0);
        self->mark_b = 1;
        *IMPL(A, last)(self) = IMPL(B, init)(DEQ_BUCKET_SIZE);
    }
    else
    {
        B* page = *IMPL(A, first)(self);
        if(page->a == 0)
        {
            if(self->mark_a == 0)
                IMPL(A, alloc)(self, 2 * self->capacity, self->mark_a);
            self->mark_a -= 1;
            *IMPL(A, first)(self) = IMPL(B, init)(DEQ_BUCKET_SIZE);
        }
    }
    B* page = *IMPL(A, first)(self);
    page->a -= 1;
    self->size += 1;
    page->value[page->a] = value;
}

static inline void
IMPL(A, push_back)(A* self, T value)
{
    if(self->capacity == 0)
    {
        IMPL(A, alloc)(self, 1, 0);
        self->mark_b = 1;
        *IMPL(A, last)(self) = IMPL(B, init)(0);
    }
    else
    {
        B* page = *IMPL(A, last)(self);
        if(page->b == DEQ_BUCKET_SIZE)
        {
            if(self->mark_b == self->capacity)
                IMPL(A, alloc)(self, 2 * self->capacity, self->mark_b);
            self->mark_b += 1;
            *IMPL(A, last)(self) = IMPL(B, init)(0);
        }
    }
    B* page = *IMPL(A, last)(self);
    page->value[page->b] = value;
    page->b += 1;
    self->size += 1;
}

static inline void
IMPL(A, pop_back)(A* self)
{
    B* page = *IMPL(A, last)(self);
    page->b -= 1;
    self->size -= 1;
    if(self->free)
    {
        T* ref = &page->value[page->b];
        self->free(ref);
    }
    if(page->b == page->a)
    {
        free(page);
        self->mark_b -= 1;
    }
}

static inline void
IMPL(A, pop_front)(A* self)
{
    B* page = *IMPL(A, first)(self);
    if(self->free)
    {
        T* ref = &page->value[page->a];
        self->free(ref);
    }
    page->a += 1;
    self->size -= 1;
    if(page->a == page->b)
    {
        free(page);
        self->mark_a += 1;
    }
}

static inline void
IMPL(A, erase)(A* self, T* position)
{
    static T zero;
    size_t index = position - IMPL(A, begin)(self);
    IMPL(A, set)(self, index, zero);
    void (*saved)(T*) = self->free;
    self->free = NULL;
    if(index < self->size / 2)
    {
        for(size_t i = index; i > 0; i--)
            *IMPL(A, at)(self, i) = *IMPL(A, at)(self, i - 1);
        IMPL(A, pop_front)(self);
    }
    else
    {
        for(size_t i = index; i < self->size - 1; i++)
            *IMPL(A, at)(self, i) = *IMPL(A, at)(self, i + 1);
        IMPL(A, pop_back)(self);
    }
    self->free = saved;
}

static inline void
IMPL(A, insert)(A* self, T* position, T value)
{
    if(self->size > 0)
    {
        size_t index = position - IMPL(A, begin)(self);
        void (*saved)(T*) = self->free;
        self->free = NULL;
        if(index < self->size / 2)
        {
            IMPL(A, push_front)(self, *IMPL(A, at)(self, 0));
            for(size_t i = 0; i < index; i++)
                *IMPL(A, at)(self, i) = *IMPL(A, at)(self, i + 1);
        }
        else
        {
            IMPL(A, push_back)(self, *IMPL(A, at)(self, self->size - 1));
            for(size_t i = self->size - 1; i > index; i--)
                *IMPL(A, at)(self, i) = *IMPL(A, at)(self, i - 1);
        }
        *IMPL(A, at)(self, index) = value;
        self->free = saved;
    }
    else
        IMPL(A, push_back)(self, value);
}

static inline void
IMPL(A, resize)(A* self, size_t size)
{
    static T zero;
    if(size != self->size)
        while(size != self->size)
            (size < self->size)
                ? IMPL(A, pop_back)(self)
                : IMPL(A, push_back)(self, self->init_default ? self->init_default() : zero);
}

static inline void
IMPL(A, assign)(A* self, size_t size, T value)
{
    IMPL(A, resize)(self, size);
    for(size_t i = 0; i < size; i++)
        IMPL(A, set)(self, i, i == 0 ? value : self->copy(&value));
}

static inline void
IMPL(A, clear)(A* self)
{
    while(!IMPL(A, empty)(self))
        IMPL(A, pop_back)(self);
}

static inline void
IMPL(A, free)(A* self)
{
    static A zero;
    IMPL(A, clear)(self);
    free(self->pages);
    *self = zero;
}

static inline void
IMPL(A, swap)(A* self, A* other)
{
    A temp = *self;
    *self = *other;
    *other = temp;
}

static inline A
IMPL(A, copy)(A* self)
{
    A other = IMPL(A, init)();
    while(other.size < self->size)
    {
        T* value = IMPL(A, at)(self, other.size);
        IMPL(A, push_back)(&other, other.copy ? other.copy(value) : *value);
    }
    return other;
}

// KNR2 (PAGE 87)

static inline void
IMPL(A, ranged_sort)(A* self, int64_t a, int64_t b, int compare(T*, T*))
{
    if(a >= b)
        return;
    SWAP(self, a, (a + b) / 2);
    int64_t z = a;
    for(int64_t i = a + 1; i <= b; i++)
        if(compare(IMPL(A, at)(self, a), IMPL(A, at)(self, i)))
        {
            z += 1;
            SWAP(self, z, i);
        }
    SWAP(self, a, z);
    IMPL(A, ranged_sort)(self, a, z - 1, compare);
    IMPL(A, ranged_sort)(self, z + 1, b, compare);
}

static inline void
IMPL(A, sort)(A* self, int compare(T*, T*))
{
    IMPL(A, ranged_sort)(self, 0, self->size - 1, compare);
}

static inline void
IMPL(I, step)(I* self)
{
    self->index = self->index_next;
    if(self->index == self->index_last)
        self->done = 1;
    else
    {
        self->ref = IMPL(A, at)(self->container, self->index);
        self->index_next += 1;
    }
}

static inline I
IMPL(I, range)(A* container, T* begin, T* end)
{
    static I zero;
    I self = zero;
    if(begin && end)
    {
        self.container = container;
        self.step = IMPL(I, step);
        self.index = begin - IMPL(A, begin)(container);
        self.index_next = self.index + 1;
        self.index_last = container->size - (IMPL(A, end)(container) - end);
        self.ref = IMPL(A, at)(container, self.index);
    }
    else
        self.done = 1;
    return self;
}

static inline I
IMPL(I, each)(A* a)
{
    return IMPL(A, empty)(a)
        ? IMPL(I, range)(a, NULL, NULL)
        : IMPL(I, range)(a, IMPL(A, begin)(a), IMPL(A, end)(a));
}

static inline void
IMPL(A, remove_if)(A* self, int (*match)(T*))
{
    foreach(A, self, it,
        if(match(it.ref))
        {
            IMPL(A, erase)(self, IMPL(A, begin)(self) + it.index);
            it.index_next = it.index;
            it.index_last -= 1;
        }
    );
}

static inline int
IMPL(A, equal)(A* self, A* other, int equal(T*, T*))
{
    if(self->size != other->size)
        return 0;
    I a = IMPL(I, each)(self);
    I b = IMPL(I, each)(other);
    while(!a.done && !b.done)
    {
        if(!equal(a.ref, b.ref))
            return 0;
        a.step(&a);
        b.step(&b);
    }
    return 1;
}

#undef T
#undef A
#undef B
#undef I

#undef DEQ_BUCKET_SIZE

#undef SWAP
