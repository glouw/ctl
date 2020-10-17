#include <ctl.h>

#define CTL_A  CTL_TEMP(CTL_T, lst)
#define CTL_B  CTL_IMPL(CTL_A, node)
#define CTL_I  CTL_IMPL(CTL_A, it)
#define CTL_TZ CTL_IMPL(CTL_T, zero)
#define CTL_AZ CTL_IMPL(CTL_A, zero)
#define CTL_IZ CTL_IMPL(CTL_I, zero)

typedef struct CTL_B
{
    CTL_T value;
    struct CTL_B* prev;
    struct CTL_B* next;
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
}
CTL_A;

typedef struct CTL_I
{
    CTL_T* ref;
    void (*step)(struct CTL_I*);
    CTL_B* begin;
    CTL_B* node;
    CTL_B* next;
    CTL_B* end;
    size_t step_size;
    bool done;
}
CTL_I;

static inline size_t
CTL_IMPL(CTL_A, size)(CTL_A* self)
{
    return self->size;
}

static inline bool
CTL_IMPL(CTL_A, empty)(CTL_A* self)
{
    return CTL_IMPL(CTL_A, size)(self) == 0;
}

static inline CTL_T*
CTL_IMPL(CTL_A, front)(CTL_A* self)
{
    return &self->head->value;
}

static inline CTL_T*
CTL_IMPL(CTL_A, back)(CTL_A* self)
{
    return &self->tail->value;
}

static inline CTL_B*
CTL_IMPL(CTL_A, begin)(CTL_A* self)
{
    return self->head;
}

static inline CTL_B*
CTL_IMPL(CTL_A, end)(CTL_A* self)
{
    return self->tail;
}

static inline CTL_A
CTL_IMPL(CTL_A, init)(void)
{
    static CTL_A CTL_AZ;
    CTL_A self = CTL_AZ;
    self.init_default = CTL_IMPL(CTL_T, init_default);
    self.free = CTL_IMPL(CTL_T, free);
    self.copy = CTL_IMPL(CTL_T, copy);
    return self;
}

static inline CTL_B*
CTL_IMPL(CTL_B, init)(CTL_T value)
{
    static CTL_B zero;
    CTL_B* self = (CTL_B*) malloc(sizeof(*self));
    *self = zero;
    self->value = value;
    return self;
}

static inline void
CTL_IMPL(CTL_A, push_back)(CTL_A* self, CTL_T value)
{
    CTL_B* node = CTL_IMPL(CTL_B, init)(value);
    if(CTL_IMPL(CTL_A, empty)(self))
        self->head = self->tail = node;
    else
    {
        node->prev = self->tail;
        self->tail->next = node;
        self->tail = node;
    }
    self->size += 1;
}

static inline void
CTL_IMPL(CTL_A, push_front)(CTL_A* self, CTL_T value)
{
    CTL_B* node = CTL_IMPL(CTL_B, init)(value);
    if(CTL_IMPL(CTL_A, empty)(self))
        self->head = self->tail = node;
    else
    {
        node->next = self->head;
        self->head->prev = node;
        self->head = node;
    }
    self->size += 1;
}

static inline void
CTL_IMPL(CTL_A, erase)(CTL_A* self, CTL_B* node)
{
    if(node == self->tail) self->tail = self->tail->prev;
    if(node == self->head) self->head = self->head->next;
    if(node->prev) node->prev->next = node->next;
    if(node->next) node->next->prev = node->prev;
    if(self->free)
        self->free(&node->value);
    free(node);
    self->size -= 1;
}

static inline void
CTL_IMPL(CTL_A, pop_back)(CTL_A* self)
{
    CTL_IMPL(CTL_A, erase)(self, self->tail);
}

static inline void
CTL_IMPL(CTL_A, pop_front)(CTL_A* self)
{
    CTL_IMPL(CTL_A, erase)(self, self->head);
}

static inline void
CTL_IMPL(CTL_A, insert)(CTL_A* self, CTL_B* at, CTL_T value)
{
    CTL_B* node = CTL_IMPL(CTL_B, init)(value);
    if(at->prev)
        at->prev->next = node;
    node->next = at;
    node->prev = at->prev;
    at->prev = node;
    if(at == self->head)
        self->head = node;
    self->size += 1;
}

static inline void
CTL_IMPL(CTL_A, free)(CTL_A* self)
{
    while(!CTL_IMPL(CTL_A, empty)(self))
        CTL_IMPL(CTL_A, pop_back)(self);
}

static inline void
CTL_IMPL(CTL_A, clear)(CTL_A* self)
{
    CTL_IMPL(CTL_A, free)(self);
}

static inline void
CTL_IMPL(CTL_A, resize)(CTL_A* self, size_t size)
{
    static CTL_T zero;
    if(size != self->size)
    {
        while(self->size != size)
            if(size < self->size)
                CTL_IMPL(CTL_A, pop_back)(self);
            else
                CTL_IMPL(CTL_A, push_back)(self, self->init_default ? self->init_default() : zero);
    }
}

static inline void
CTL_IMPL(CTL_A, assign)(CTL_A* self, size_t size, CTL_T value)
{
    CTL_IMPL(CTL_A, clear)(self);
    for(size_t i = 0; i < size; i++)
        CTL_IMPL(CTL_A, push_back)(self, i == 0 ? value : self->copy ? self->copy(&value) : value);
}

static inline void
CTL_IMPL(CTL_A, swap)(CTL_A* self, CTL_A* other)
{
    CTL_A temp = *self;
    *self = *other;
    *other = temp;
}

static inline void
CTL_IMPL(CTL_A, reverse)(CTL_A* self)
{
    CTL_B* node = self->head;
    while(node)
    {
        CTL_B* next = node->next;
        CTL_B* prev = node->prev;
        node->prev = next;
        node->next = prev;
        node = next;
    }
    CTL_B* tail = self->tail;
    CTL_B* head = self->head;
    self->tail = head;
    self->head = tail;
}

static inline CTL_A
CTL_IMPL(CTL_A, copy)(CTL_A* self)
{
    CTL_A other = CTL_IMPL(CTL_A, init)();
    for(CTL_B* node = self->head; node; node = node->next)
        CTL_IMPL(CTL_A, push_back)(&other, self->copy ? self->copy(&node->value) : node->value);
    return other;
}

static inline void
CTL_IMPL(CTL_I, step)(CTL_I* self)
{
    for(size_t i = 0; i < self->step_size; i++)
        if(self->node == self->end)
            self->done = true;
        else
        {
            self->node = self->next;
            self->ref = &self->node->value;
            self->next = self->node->next;
        }
}

static inline CTL_I
CTL_IMPL(CTL_I, by)(CTL_B* begin, CTL_B* end, size_t step_size)
{
    static CTL_I CTL_IZ;
    CTL_I self = CTL_IZ;
    self.step = CTL_IMPL(CTL_I, step);
    self.begin = begin;
    self.node = begin;
    self.next = begin->next;
    self.end = end;
    self.step_size = step_size;
    self.ref = &self.node->value;
    return self;
}

static inline CTL_I
CTL_IMPL(CTL_I, each)(CTL_A* a)
{
    return CTL_IMPL(CTL_I, by)(CTL_IMPL(CTL_A, begin)(a), CTL_IMPL(CTL_A, end)(a), 1);
}

#undef CTL_T
#undef CTL_A
#undef CTL_B
#undef CTL_I
#undef CTL_TZ
#undef CTL_AZ
#undef CTL_IZ

//Operations:

//splice
//    Transfer elements from list to list (public member function )

//remove
//    Remove elements with specific value (public member function )

//remove_if
//    Remove elements fulfilling condition (public member function template )

//unique
//    Remove duplicate values (public member function )

//sort
//    Sort elements in container (public member function )

//merge
//    Merge sorted lists (public member function )
