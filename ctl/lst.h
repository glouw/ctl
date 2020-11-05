#include <ctl.h>

#define A TEMP(T, lst)
#define B IMPL(A, node)
#define I IMPL(A, it)

typedef struct B
{
    struct B* prev;
    struct B* next;
    T value;
}
B;

typedef struct
{
    T (*init_default)(void);
    void (*free)(T*);
    T (*copy)(T*);
    B* head;
    B* tail;
    size_t size;
}
A;

typedef struct I
{
    void (*step)(struct I*);
    T* ref;
    B* begin;
    B* node;
    B* next;
    B* end;
    int done;
}
I;

static inline A
IMPL(A, init)(void)
{
    static A zero;
    A self = zero;
#ifdef P
#undef P
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
IMPL(B, init)(T value)
{
    B* self = (B*) malloc(sizeof(B));
    self->prev = self->next = NULL;
    self->value = value;
    return self;
}

static inline int
IMPL(A, empty)(A* self)
{
    return self->size == 0;
}

static inline T*
IMPL(A, front)(A* self)
{
    return &self->head->value;
}

static inline T*
IMPL(A, back)(A* self)
{
    return &self->tail->value;
}

static inline B*
IMPL(A, begin)(A* self)
{
    return self->head;
}

static inline B*
IMPL(A, end)(A* self)
{
    (void) self;
    return NULL;
}

static inline void
IMPL(A, disconnect)(A* self, B* node)
{
    if(node == self->tail) self->tail = self->tail->prev;
    if(node == self->head) self->head = self->head->next;
    if(node->prev) node->prev->next = node->next;
    if(node->next) node->next->prev = node->prev;
    node->prev = node->next = NULL;
    self->size -= 1;
}

static inline void
IMPL(A, connect)(A* self, B* position, B* node, int before)
{
    if(IMPL(A, empty)(self))
        self->head = self->tail = node;
    else
    if(before)
    {
        node->next = position;
        node->prev = position->prev;
        if(position->prev)
            position->prev->next = node;
        position->prev = node;
        if(position == self->head)
            self->head = node;
    }
    else
    {
        node->prev = position;
        node->next = position->next;
        if(position->next)
            position->next->prev = node;
        position->next = node;
        if(position == self->tail)
            self->tail = node;
    }
    self->size += 1;
}

static inline void
IMPL(A, push_back)(A* self, T value)
{
    B* node = IMPL(B, init)(value);
    IMPL(A, connect)(self, self->tail, node, 0);
}

static inline void
IMPL(A, push_front)(A* self, T value)
{
    B* node = IMPL(B, init)(value);
    IMPL(A, connect)(self, self->head, node, 1);
}

static inline void
IMPL(A, transfer)(A* self, A* other, B* position, B* node, int before)
{
    IMPL(A, disconnect)(other, node);
    IMPL(A, connect)(self, position, node, before);
}

static inline void
IMPL(A, erase)(A* self, B* node)
{
    IMPL(A, disconnect)(self, node);
    if(self->free)
        self->free(&node->value);
    free(node);
}

static inline void
IMPL(A, pop_back)(A* self)
{
    IMPL(A, erase)(self, self->tail);
}

static inline void
IMPL(A, pop_front)(A* self)
{
    IMPL(A, erase)(self, self->head);
}

static inline void
IMPL(A, insert)(A* self, B* position, T value)
{
    B* node = IMPL(B, init)(value);
    IMPL(A, connect)(self, position, node, 1);
}

static inline void
IMPL(A, free)(A* self)
{
    while(!IMPL(A, empty)(self))
        IMPL(A, pop_back)(self);
}

static inline void
IMPL(A, clear)(A* self)
{
    IMPL(A, free)(self);
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
    for(B* node = self->head; node; node = node->next)
        IMPL(A, push_back)(&other, self->copy ? self->copy(&node->value) : node->value);
    return other;
}

static inline void
IMPL(I, step)(I* self)
{
    if(self->next == self->end)
        self->done = 1;
    else
    {
        self->node = self->next;
        self->ref = &self->node->value;
        self->next = self->node->next;
    }
}

static inline I
IMPL(I, range)(B* begin, B* end)
{
    static I zero;
    I self = zero;
    if(begin)
    {
        self.step = IMPL(I, step);
        self.begin = begin;
        self.end = end;
        self.next = begin->next;
        self.node = begin;
        self.ref = &begin->value;
    }
    else
        self.done = 1;
    return self;
}

static inline I
IMPL(I, each)(A* a)
{
    return IMPL(I, range)(IMPL(A, begin)(a), IMPL(A, end)(a));
}

static inline void
IMPL(A, assign)(A* self, size_t size, T value)
{
    IMPL(A, resize)(self, size);
    size_t index = 0;
    foreach(A, self, it, {
        if(self->free)
            self->free(it.ref);
        *it.ref = (index == 0) ? value : self->copy ? self->copy(&value) : value;
        index += 1;
    });
}

static inline void
IMPL(A, reverse)(A* self)
{
    foreach(A, self, it, {
        B* next = it.node->next;
        B* prev = it.node->prev;
        it.node->prev = next;
        it.node->next = prev;
    });
    B* tail = self->tail;
    B* head = self->head;
    self->tail = head;
    self->head = tail;
}

static inline void
IMPL(A, remove_if)(A* self, int (*equal)(T*))
{
    foreach(A, self, it, {
        if(equal(it.ref))
            IMPL(A, erase)(self, it.node);
    });
}

static inline void
IMPL(A, splice)(A* self, B* position, A* other)
{
    if(self->size == 0 && position == NULL)
        IMPL(A, swap)(self, other);
    else
    {
        foreach(A, other, it, {
            IMPL(A, transfer)(self, other, position, it.node, 1);
        });
    }
}

static inline void
IMPL(A, merge)(A* self, A* other, int compare(T*, T*))
{
    if(IMPL(A, empty)(self))
        IMPL(A, swap)(self, other);
    else
    {
        for(B* node = self->head; node; node = node->next)
            while(!IMPL(A, empty)(other) && compare(&node->value, &other->head->value))
                IMPL(A, transfer)(self, other, node, other->head, 1);
        while(!IMPL(A, empty)(other)) // REMAINDER.
            IMPL(A, transfer)(self, other, self->tail, other->head, 0);
    }
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

static inline void
IMPL(A, sort)(A* self, int compare(T*, T*))
{
    if(self->size > 1)
    {
        A carry = IMPL(A, init)();
        A temp[64];
        for(size_t i = 0; i < LEN(temp); i++)
            temp[i] = IMPL(A, init)();
        A* fill = temp;
        A* counter = NULL;
        do
        {
            IMPL(A, transfer)(&carry, self, carry.head, self->head, 1);
            for(counter = temp; counter != fill && !IMPL(A, empty)(counter); counter++)
            {
                IMPL(A, merge)(counter, &carry, compare);
                IMPL(A, swap)(&carry, counter);
            }
            IMPL(A, swap)(&carry, counter);
            if(counter == fill)
                fill++;
        }
        while(!IMPL(A, empty)(self));
        for(counter = temp + 1; counter != fill; counter++)
            IMPL(A, merge)(counter, counter - 1, compare);
        IMPL(A, swap)(self, fill - 1);
    }
}

static inline void
IMPL(A, unique)(A* self, int equal(T*, T*))
{
    foreach(A, self, it,
        if(it.next && equal(it.ref, &it.next->value))
            IMPL(A, erase)(self, it.node);
    );
}

#undef T
#undef A
#undef B
#undef I
