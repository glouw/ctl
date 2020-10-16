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
    void (*free)(CTL_T*);
    CTL_T (*copy)(CTL_T*);
    CTL_B* head;
    CTL_B* tail;
    size_t size;
}
CTL_A;

typedef struct CTL_I
{
    CTL_A* container;
    CTL_T* ref;
    void (*step)(struct CTL_I*);
    CTL_B* begin;
    CTL_B* node;
    CTL_B* end;
    size_t index;
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
CTL_IMPL(CTL_A, pop_back)(CTL_A* self)
{
    static CTL_A CTL_AZ;
    CTL_B* prev = self->tail->prev;
    if(self->free)
        self->free(CTL_IMPL(CTL_A, back)(self));
    free(self->tail);
    self->size -= 1;
    if(self->size > 0)
    {
        self->tail = prev;
        self->tail->next = NULL;
    }
    else
        *self = CTL_AZ;
}

static inline void
CTL_IMPL(CTL_A, pop_front)(CTL_A* self)
{
    static CTL_A CTL_AZ;
    CTL_B* next = self->head->next;
    if(self->free)
        self->free(CTL_IMPL(CTL_A, front)(self));
    free(self->head);
    self->size -= 1;
    if(self->size > 0)
    {
        self->head = next;
        self->head->prev = NULL;
    }
    else
        *self = CTL_AZ;
}

static inline void
CTL_IMPL(CTL_A, free)(CTL_A* self)
{
    while(!CTL_IMPL(CTL_A, empty)(self))
        CTL_IMPL(CTL_A, pop_front)(self);
}

static inline void
CTL_IMPL(CTL_I, step)(CTL_I* self)
{
    CTL_B* node = self->node;
    for(size_t steps = 0; steps < self->step_size; steps++)
    {
        node = node->next;
        self->index += 1;
        if(node == NULL || node == self->end)
        {
            self->done = true;
            break;
        }
    }
    self->node = node;
    self->ref = &node->value;
}

static inline CTL_I
CTL_IMPL(CTL_I, by)(CTL_A* container, CTL_B* begin, CTL_B* end, size_t step_size)
{
    static CTL_I CTL_IZ;
    CTL_I self = CTL_IZ;
    self.container = container;
    self.step = CTL_IMPL(CTL_I, step);
    self.begin = begin;
    self.node = begin;
    self.end = end;
    self.step_size = step_size;
    self.ref = &self.node->value;
    return self;
}

static inline CTL_I
CTL_IMPL(CTL_I, each)(CTL_A* a)
{
    size_t step_size = 1;
    return CTL_IMPL(CTL_I, by)(a, CTL_IMPL(CTL_A, begin)(a), CTL_IMPL(CTL_A, end)(a), step_size);
}

#undef CTL_T
#undef CTL_A
#undef CTL_B
#undef CTL_I
#undef CTL_TZ
#undef CTL_AZ
#undef CTL_IZ

//Capacity:



//Modifiers:

//assign
//    Assign new content to container (public member function )

//insert
//    Insert elements (public member function )

//erase
//    Erase elements (public member function )

//swap
//    Swap content (public member function )

//resize
//    Change size (public member function )

//clear
//    Clear content (public member function )


//Operations:

//splice
//    Transfer elements from list to list (public member function )

//remove
//    Remove elements with specific value (public member function )

//remove_if
//    Remove elements fulfilling condition (public member function template )

//unique
//    Remove duplicate values (public member function )

//merge
//    Merge sorted lists (public member function )

//sort
//    Sort elements in container (public member function )

//reverse
//    Reverse the order of elements (public member function )
//
