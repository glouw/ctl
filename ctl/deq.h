#include <ctl.h>

#include <stdio.h>

#define CTL_A  CTL_TEMP(CTL_T, deq)
#define CTL_B  CTL_IMPL(CTL_A, bucket)

#define CTL_DEQ_BUCKET_SIZE (8)

typedef struct CTL_B
{
    CTL_T value[CTL_DEQ_BUCKET_SIZE];
    int16_t a;
    int16_t b;
}
CTL_B;

typedef struct
{
    CTL_T (*init_default)(void);
    void (*free)(CTL_T*);
    CTL_T (*copy)(CTL_T*);
    CTL_B** pages;
    size_t mark_a;
    size_t mark_b;
    size_t capacity;
    size_t size;
}
CTL_A;

static inline bool
CTL_IMPL(CTL_A, empty)(CTL_A* self)
{
    return self->size == 0;
}

static inline CTL_A
CTL_IMPL(CTL_A, init)(void)
{
    static CTL_A zero;
    CTL_A self = zero;
#ifdef CTL_P
#undef CTL_P
#else
    self.init_default = CTL_IMPL(CTL_T, init_default);
    self.free = CTL_IMPL(CTL_T, free);
    self.copy = CTL_IMPL(CTL_T, copy);
#endif
    return self;
}

static inline CTL_B*
CTL_IMPL(CTL_B, init)(size_t cut)
{
    CTL_B* self = (CTL_B*) malloc(sizeof(CTL_B));
    self->a = self->b = cut;
    return self;
}

static inline CTL_B**
CTL_IMPL(CTL_A, first)(CTL_A* self)
{
    return &self->pages[self->mark_a];
}

static inline CTL_B**
CTL_IMPL(CTL_A, last)(CTL_A* self)
{
    return &self->pages[self->mark_b - 1];
}

static inline CTL_T*
CTL_IMPL(CTL_A, at)(CTL_A* self, size_t index)
{
    CTL_B* first = *CTL_IMPL(CTL_A, first)(self);
    size_t actual = index + first->a;
    size_t look = self->mark_a + actual / CTL_DEQ_BUCKET_SIZE;
    CTL_B* page = self->pages[look];
    size_t cut = actual % CTL_DEQ_BUCKET_SIZE;
    CTL_T* ref = &page->value[cut];
    printf("%lu %lu %lu\n", look, cut, *ref);
    return ref;
}

static inline void
CTL_IMPL(CTL_A, set)(CTL_A* self, size_t index, CTL_T value)
{
    CTL_T* ref = CTL_IMPL(CTL_A, at)(self, index);
    if(self->free)
        self->free(ref);
    *ref = value;
}

static inline void
CTL_IMPL(CTL_A, reserve)(CTL_A* self, size_t capacity, size_t shift_from)
{
    self->capacity = capacity;
    self->pages = (CTL_B**) realloc(self->pages, capacity * sizeof(CTL_B*));
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
CTL_IMPL(CTL_A, push_front)(CTL_A* self, CTL_T value)
{
    if(self->capacity == 0)
    {
        CTL_IMPL(CTL_A, reserve)(self, 1, 0);
        self->mark_b = 1;
        *CTL_IMPL(CTL_A, last)(self) = CTL_IMPL(CTL_B, init)(CTL_DEQ_BUCKET_SIZE);
    }
    else
    {
        CTL_B* page = *CTL_IMPL(CTL_A, first)(self);
        if(page->a == 0)
        {
            if(self->mark_a == 0)
                CTL_IMPL(CTL_A, reserve)(self, 2 * self->capacity, self->mark_a);
            self->mark_a -= 1;
            *CTL_IMPL(CTL_A, first)(self) = CTL_IMPL(CTL_B, init)(CTL_DEQ_BUCKET_SIZE);
        }
    }
    CTL_B* page = *CTL_IMPL(CTL_A, first)(self);
    page->a -= 1;
    self->size += 1;
    page->value[page->a] = value;
}

static inline void
CTL_IMPL(CTL_A, push_back)(CTL_A* self, CTL_T value)
{
    if(self->capacity == 0)
    {
        CTL_IMPL(CTL_A, reserve)(self, 1, 0);
        self->mark_b = 1;
        *CTL_IMPL(CTL_A, last)(self) = CTL_IMPL(CTL_B, init)(0);
    }
    else
    {
        CTL_B* page = *CTL_IMPL(CTL_A, last)(self);
        if(page->b == CTL_DEQ_BUCKET_SIZE)
        {
            if(self->mark_b == self->capacity)
                CTL_IMPL(CTL_A, reserve)(self, 2 * self->capacity, self->mark_b);
            self->mark_b += 1;
            *CTL_IMPL(CTL_A, last)(self) = CTL_IMPL(CTL_B, init)(0);
        }
    }
    CTL_B* page = *CTL_IMPL(CTL_A, last)(self);
    page->value[page->b] = value;
    page->b += 1;
    self->size += 1;
}

static inline void
CTL_IMPL(CTL_A, pop_back)(CTL_A* self)
{
    CTL_B* page = *CTL_IMPL(CTL_A, last)(self);
    page->b -= 1;
    self->size -= 1;
    if(self->free)
    {
        CTL_T* ref = &page->value[page->b];
        self->free(ref);
    }
    if(page->b == page->a)
    {
        free(page);
        self->mark_b -= 1;
    }
}

static inline void
CTL_IMPL(CTL_A, pop_front)(CTL_A* self)
{
    CTL_B* page = *CTL_IMPL(CTL_A, first)(self);
    if(self->free)
    {
        CTL_T* ref = &page->value[page->a];
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
CTL_IMPL(CTL_A, free)(CTL_A* self)
{
    while(!CTL_IMPL(CTL_A, empty)(self))
        CTL_IMPL(CTL_A, pop_back)(self);
    free(self->pages);
    self->pages = NULL;
    self->mark_a = self->mark_b = self->capacity = 0;
}

#undef CTL_DEQ_BUCKET_SIZE

#undef CTL_T
#undef CTL_A
#undef CTL_B
