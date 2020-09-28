/*
 * ARRAY
 *
 * VALUES ARE STACK ALLOCATED.
 *
 */

#include <ctl/ctl.h>

#define _A TEMPLATE(T, array)
#define  A PASTE(_A, PASTE(_, S))

typedef struct
{
    void (*destruct)(T*);
    T (*copy)(T*);
    T (*move)(T*);
    T value[S];
    int size;
}
A;

static A
IMPL(A, init)(void (*destruct)(T*), T (*copy)(T*), T (*move)(T*))
{
    static A zero;
    A self = zero;
    self.destruct = destruct;
    self.copy = copy;
    self.move = move;
    self.size = S;
    return self;
}

static A
IMPL(A, copy)(A* self)
{
    A copied = *self;
    if(self->copy)
    {
        int i;
        for(i = 0; i < copied.size; i++)
            copied.value[i] = self->copy(&self->value[i]);
    }
    return copied;
}

static A
IMPL(A, move)(A* self)
{
    static A zero;
    A moved = *self;
    if(self->move)
    {
        int i;
        for(i = 0; i < moved.size; i++)
            moved.value[i] = moved.move(&self->value[i]);
    }
    *self = zero;
    return moved;
}

static void
IMPL(A, check)(A* self, int index)
{
    assert(index >= 0);
    assert(index < self->size);
}

static void
IMPL(A, del)(A* self, int index)
{
    static T zero;
    IMPL(A, check)(self, index);
    if(self->destruct)
        self->destruct(&self->value[index]);
    self->value[index] = zero;
}

static void
IMPL(A, set)(A* self, int index, T value)
{
    IMPL(A, del)(self, index);
    self->value[index] = value;
}

static void
IMPL(A, destruct)(A* self)
{
    int i;
    for(i = 0; i < self->size; i++)
        IMPL(A, del)(self, i);
}

#undef A
#undef S
#undef T
