//
// Double Ended Queue
//

#ifndef T
#error "Template type T undefined for <deq.h>"
#endif

#include <ctl.h>

#define A JOIN(deq, T)
#define B JOIN(A, bucket)
#define Z JOIN(A, it)

#define DEQ_BUCKET_SIZE (512)

typedef struct B
{
    T value[DEQ_BUCKET_SIZE];
    int16_t a;
    int16_t b;
}
B;

typedef struct A
{
    void (*free)(T*);
    T (*copy)(T*);
    B** pages;
    size_t mark_a;
    size_t mark_b;
    size_t capacity;
    size_t size;
}
A;

typedef struct Z
{
    void (*step)(struct Z*);
    A* container;
    T* ref;
    size_t index;
    size_t index_next;
    size_t index_last;
    int done;
}
Z;

static inline B**
JOIN(A, first)(A* self)
{
    return &self->pages[self->mark_a];
}

static inline B**
JOIN(A, last)(A* self)
{
    return &self->pages[self->mark_b - 1];
}

static inline T*
JOIN(A, at)(A* self, size_t index)
{
    if(self->size == 0)
        return NULL;
    else
    {
        B* first = *JOIN(A, first)(self);
        size_t actual = index + first->a;
        size_t q = actual / DEQ_BUCKET_SIZE;
        size_t r = actual % DEQ_BUCKET_SIZE;
        B* page = self->pages[self->mark_a + q];
        return &page->value[r];
    }
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
    self->index = self->index_next;
    if(self->index == self->index_last)
        self->done = 1;
    else
    {
        self->ref = JOIN(A, at)(self->container, self->index);
        self->index_next += 1;
    }
}

static inline Z
JOIN(Z, range)(A* container, T* begin, T* end)
{
    static Z zero;
    Z self = zero;
    if(begin && end)
    {
        self.container = container;
        self.step = JOIN(Z, step);
        self.index = begin - JOIN(A, begin)(container);
        self.index_next = self.index + 1;
        self.index_last = container->size - (JOIN(A, end)(container) - end);
        self.ref = JOIN(A, at)(container, self.index);
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

static inline B*
JOIN(B, init)(size_t cut)
{
    B* self = (B*) malloc(sizeof(B));
    self->a = self->b = cut;
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
JOIN(A, alloc)(A* self, size_t capacity, size_t shift_from)
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
JOIN(A, push_front)(A* self, T value)
{
    if(JOIN(A, empty)(self))
    {
        self->mark_a = 0;
        self->mark_b = 1;
        JOIN(A, alloc)(self, 1, 0);
        *JOIN(A, last)(self) = JOIN(B, init)(DEQ_BUCKET_SIZE);
    }
    else
    {
        B* page = *JOIN(A, first)(self);
        if(page->a == 0)
        {
            if(self->mark_a == 0)
                JOIN(A, alloc)(self, 2 * self->capacity, self->mark_a);
            self->mark_a -= 1;
            *JOIN(A, first)(self) = JOIN(B, init)(DEQ_BUCKET_SIZE);
        }
    }
    B* page = *JOIN(A, first)(self);
    page->a -= 1;
    self->size += 1;
    page->value[page->a] = value;
}

static inline void
JOIN(A, pop_front)(A* self)
{
    B* page = *JOIN(A, first)(self);
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
JOIN(A, push_back)(A* self, T value)
{
    if(JOIN(A, empty)(self))
    {
        self->mark_a = 0;
        self->mark_b = 1;
        JOIN(A, alloc)(self, 1, 0);
        *JOIN(A, last)(self) = JOIN(B, init)(0);
    }
    else
    {
        B* page = *JOIN(A, last)(self);
        if(page->b == DEQ_BUCKET_SIZE)
        {
            if(self->mark_b == self->capacity)
                JOIN(A, alloc)(self, 2 * self->capacity, self->mark_b);
            self->mark_b += 1;
            *JOIN(A, last)(self) = JOIN(B, init)(0);
        }
    }
    B* page = *JOIN(A, last)(self);
    page->value[page->b] = value;
    page->b += 1;
    self->size += 1;
}

static inline void
JOIN(A, pop_back)(A* self)
{
    B* page = *JOIN(A, last)(self);
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
JOIN(A, erase)(A* self, size_t index)
{
    static T zero;
    JOIN(A, set)(self, index, zero);
    void (*saved)(T*) = self->free;
    self->free = NULL;
    if(index < self->size / 2)
    {
        for(size_t i = index; i > 0; i--)
            *JOIN(A, at)(self, i) = *JOIN(A, at)(self, i - 1);
        JOIN(A, pop_front)(self);
    }
    else
    {
        for(size_t i = index; i < self->size - 1; i++)
            *JOIN(A, at)(self, i) = *JOIN(A, at)(self, i + 1);
        JOIN(A, pop_back)(self);
    }
    self->free = saved;
}

static inline void
JOIN(A, insert)(A* self, size_t index, T value)
{
    if(self->size > 0)
    {
        void (*saved)(T*) = self->free;
        self->free = NULL;
        if(index < self->size / 2)
        {
            JOIN(A, push_front)(self, *JOIN(A, at)(self, 0));
            for(size_t i = 0; i < index; i++)
                *JOIN(A, at)(self, i) = *JOIN(A, at)(self, i + 1);
        }
        else
        {
            JOIN(A, push_back)(self, *JOIN(A, at)(self, self->size - 1));
            for(size_t i = self->size - 1; i > index; i--)
                *JOIN(A, at)(self, i) = *JOIN(A, at)(self, i - 1);
        }
        *JOIN(A, at)(self, index) = value;
        self->free = saved;
    }
    else
        JOIN(A, push_back)(self, value);
}

static inline void
JOIN(A, resize)(A* self, size_t size, T value)
{
    if(size != self->size)
    {
        while(size != self->size)
            if(size < self->size)
                JOIN(A, pop_back)(self);
            else
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
JOIN(A, clear)(A* self)
{
    while(!JOIN(A, empty)(self))
        JOIN(A, pop_back)(self);
}

static inline void
JOIN(A, free)(A* self)
{
    JOIN(A, clear)(self);
    free(self->pages);
    *self = JOIN(A, init)();
}

static inline A
JOIN(A, copy)(A* self)
{
    A other = JOIN(A, init)();
    while(other.size < self->size)
    {
        T* value = JOIN(A, at)(self, other.size);
        JOIN(A, push_back)(&other, other.copy(value));
    }
    return other;
}

static inline void
JOIN(A, ranged_sort)(A* self, int64_t a, int64_t b, int _compare(T*, T*))
{
    if(a >= b)
        return;
    int64_t mid = (a + b) / 2;
    SWAP(T, JOIN(A, at)(self, a), JOIN(A, at)(self, mid));
    int64_t z = a;
    for(int64_t i = a + 1; i <= b; i++)
        if(_compare(JOIN(A, at)(self, a), JOIN(A, at)(self, i)))
        {
            z += 1;
            SWAP(T, JOIN(A, at)(self, z), JOIN(A, at)(self, i));
        }
    SWAP(T, JOIN(A, at)(self, a), JOIN(A, at)(self, z));
    JOIN(A, ranged_sort)(self, a, z - 1, _compare);
    JOIN(A, ranged_sort)(self, z + 1, b, _compare);
}

static inline void
JOIN(A, sort)(A* self, int _compare(T*, T*))
{
    JOIN(A, ranged_sort)(self, 0, self->size - 1, _compare);
}

static inline size_t
JOIN(A, remove_if)(A* self, int _match(T*))
{
    size_t erases = 0;
    foreach(A, self, it)
        if(_match(it.ref))
        {
            JOIN(A, erase)(self, it.index);
            it.index_next = it.index;
            it.index_last -= 1;
            erases += 1;
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

#undef T
#undef A
#undef B
#undef Z

#undef DEQ_BUCKET_SIZE
