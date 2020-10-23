#include <ctl.h>

#define CTL_A  CTL_TEMP(CTL_T, deq)
#define CTL_B  CTL_IMPL(CTL_A, page)

#ifdef CTL_U
#define CTL_DEQ_PAGE_SIZE CTL_U
#else
#define CTL_DEQ_PAGE_SIZE (512)
#endif

typedef struct CTL_B
{
    struct CTL_B* prev;
    struct CTL_B* next;
    size_t size;
    CTL_T value[CTL_DEQ_PAGE_SIZE];
}
CTL_B;

typedef struct
{
    CTL_T (*init_default)(void);
    void (*free)(CTL_T*);
    CTL_T (*copy)(CTL_T*);
    CTL_B* head;
    CTL_B* tail;
    size_t size;
    size_t page_size;
    size_t pages;
}
CTL_A;

static inline CTL_T*
CTL_IMPL(CTL_A, at)(CTL_A* self, size_t index)
{
    size_t mid = self->pages / 2;
    size_t page = index / self->page_size;
    bool head = page < mid;
    size_t steps = (head ? page : (self->pages - page)) - 1;
    CTL_B* end = head ? self->head : self->tail;
    for(size_t i = 0; i < steps; i++)
        end = head ? end->next : end->prev;
    return &end->value[index % self->page_size];
}

static inline CTL_T*
CTL_IMPL(CTL_A, front)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, at)(self, 0);
}

static inline CTL_T*
CTL_IMPL(CTL_A, back)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, at)(self, self->size - 1);
}

static inline CTL_T*
CTL_IMPL(CTL_A, begin)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, front)(self);
}

static inline CTL_T*
CTL_IMPL(CTL_A, end)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, back)(self) + 1;
}

static inline CTL_A
CTL_IMPL(CTL_A, init)(void)
{
    static CTL_A zero;
    CTL_A self = zero;
#ifdef CTL_POD
#undef CTL_POD
#else
    self.init_default = CTL_IMPL(CTL_T, init_default);
    self.free = CTL_IMPL(CTL_T, free);
    self.copy = CTL_IMPL(CTL_T, copy);
#endif
    self.page_size = CTL_DEQ_PAGE_SIZE;
    return self;
}

static inline CTL_B*
CTL_IMPL(CTL_B, init)(void)
{
    CTL_B* self = (CTL_B*) malloc(sizeof(CTL_B));
    self->size = 0;
    self->prev = self->next = NULL;
    return self;
}

static inline bool
CTL_IMPL(CTL_A, empty)(CTL_A* self)
{
    return self->size == 0;
}

static inline void
CTL_IMPL(CTL_A, link_disconnect)(CTL_A* self, CTL_B* page)
{
    if(page == self->tail) self->tail = self->tail->prev;
    if(page == self->head) self->head = self->head->next;
    if(page->prev) page->prev->next = page->next;
    if(page->next) page->next->prev = page->prev;
    page->prev = page->next = NULL;
    self->pages -= 1;
}

static inline void
CTL_IMPL(CTL_A, link_connect)(CTL_A* self, CTL_B* position, CTL_B* page, bool before)
{
    if(CTL_IMPL(CTL_A, empty)(self))
        self->head = self->tail = page;
    else
    if(before)
    {
        page->next = position;
        page->prev = position->prev;
        if(position->prev)
            position->prev->next = page;
        position->prev = page;
        if(position == self->head)
            self->head = page;
    }
    else
    {
        page->prev = position;
        page->next = position->next;
        if(position->next)
            position->next->prev = page;
        position->next = page;
        if(position == self->tail)
            self->tail = page;
    }
    self->pages += 1;
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
CTL_IMPL(CTL_A, push_back)(CTL_A* self, CTL_T value)
{
    if(self->tail == NULL || self->tail->size == self->page_size)
    {
        CTL_B* page = CTL_IMPL(CTL_B, init)();
        CTL_IMPL(CTL_A, link_connect)(self, self->tail, page, false);
    }
    CTL_IMPL(CTL_A, set)(self, self->size, value);
    self->tail->size += 1;
    self->size += 1;
}

static inline void
CTL_IMPL(CTL_A, pop_back)(CTL_A* self)
{
    static CTL_T zero;
    self->tail->size -= 1;
    self->size -= 1;
    CTL_IMPL(CTL_A, set)(self, self->size, zero);
    if(self->tail->size == 0)
    {
        CTL_B* copy = self->tail;
        CTL_IMPL(CTL_A, link_disconnect)(self, self->tail);
        free(copy);
    }
}

static inline void
CTL_IMPL(CTL_A, free)(CTL_A* self)
{
    while(self->size > 0)
        CTL_IMPL(CTL_A, pop_back)(self);
}

#undef CTL_DEQ_PAGE_SIZE
#undef CTL_T
#undef CTL_U
#undef CTL_A
#undef CTL_B
