/*
 * DOUBLE ENDED LINKED LIST
 *
 */

#include <ctl/ctl.h>

#define A TEMPLATE(T, delist_node)
#define B TEMPLATE(T, delist)

typedef struct A
{
    T value;
    struct A* next;
    struct A* prev;
}
A;

static A*
IMPL(A, init)(T value)
{
    A* self = (A*) malloc(sizeof(A));
    self->value = value;
    self->next = NULL;
    self->prev = NULL;
    return self;
}

static void
IMPL(A, free)(A* self)
{
    free(self);
}

typedef struct B
{
    A* head;
    A* tail;
    void (*destruct)(T*);
    int size;
}
B;

static B
IMPL(B, init)(void (*destruct)(T*))
{
    B self;
    self.head = NULL;
    self.tail = NULL;
    self.destruct = destruct;
    self.size = 0;
    return self;
}

static T
IMPL(B, peak_back)(B* self)
{
    return self->tail->value;
}

static T
IMPL(B, peak_front)(B* self)
{
    return self->head->value;
}

static void
IMPL(B, insert)(B* self, T value, A* target, int before)
{
    A* node = IMPL(A, init)(value);
    if(self->size == 0)
    {
        self->head = node;
        self->tail = node;
    }
    else
    {
        if(before)
        {
            node->next = target;
            node->prev = target->prev;
            if(target->prev)
                target->prev->next = node;
            target->prev = node;
            if(target == self->head)
                self->head = node;
        }
        else
        {
            node->next = target->next;
            node->prev = target;
            if(target->next)
                target->next->prev = node;
            target->next = node;
            if(target == self->tail)
                self->tail = node;
        }
    }
    self->size += 1;
}

static void
IMPL(B, push_back)(B* self, T value)
{
    IMPL(B, insert)(self, value, self->tail, 0);
}

static void
IMPL(B, push_front)(B* self, T value)
{
    IMPL(B, insert)(self, value, self->head, 1);
}

static T
IMPL(B, pop)(B* self, A* target)
{
    T value = target->value;
    if(target == self->head) self->head = target->next;
    if(target == self->tail) self->tail = target->prev;
    if(target->prev) target->prev->next = target->next;
    if(target->next) target->next->prev = target->prev;
    IMPL(A, free)(target);
    self->size -= 1;
    return value;
}

static T
IMPL(B, pop_back)(B* self)
{
    return IMPL(B, pop)(self, self->tail);
}

static T
IMPL(B, pop_front)(B* self)
{
    return IMPL(B, pop)(self, self->head);
}

static void
IMPL(B, del)(B* self, A* target)
{
    T value = IMPL(B, pop)(self, target);
    if(self->destruct)
        self->destruct(&value);
}

static void
IMPL(B, free)(B* self)
{
    while(self->size > 0)
        IMPL(B, del)(self, self->head);
}

static void
IMPL(B, sort)(B* self, int (*compare)(const void*, const void*))
{
    A* node;
    T* cache = (T*) malloc(self->size * sizeof(T));
    int index = 0;
    for(node = self->head; node; node = node->next)
        cache[index++] = node->value;
    qsort(cache, self->size, sizeof(T), compare);
    index = 0;
    for(node = self->head; node; node = node->next)
        node->value = cache[index++];
    free(cache);
}

#undef A
#undef B
#undef T
