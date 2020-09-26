#include <ctl/ctl.h>

#define A TEMPLATE(T, list_node)
#define B TEMPLATE(T, list)

typedef struct A
{
    T data;
    struct A* next;
    struct A* prev;
}
A;

static A*
IMPL(A, init)(T data)
{
    A* b = (A*) malloc(sizeof(T));
    b->data = data;
    b->next = NULL;
    b->prev = NULL;
    return b;
}

static void
IMPL(A, free)(A* b)
{
    free(b);
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

static int
IMPL(B, empty)(B* self, A* node)
{
    return self->size == 0;
}

static void
IMPL(B, setup)(B* self, A* node)
{
    if(IMPL(B, empty)(self, node))
    {
        self->head = node;
        self->tail = node;
    }
}

static void
IMPL(B, push_back)(B* self, A* node)
{
    IMPL(B, setup)(self, node);
    node->prev = self->tail;
    node->prev->next = node;
    self->tail = node;
    self->size += 1;
}

static void
IMPL(B, push_front)(B* self, A* node)
{
    IMPL(B, setup)(self, node);
    node->next = self->head;
    node->next->prev = node;
    self->head = node;
    self->size += 1;
}

static T
IMPL(B, pop)(B* self, A* node)
{
    A* prev = node->prev;
    A* next = node->next;
    T data = node->data;
    if(node == self->head) self->head = next;
    if(node == self->tail) self->tail = prev;
    if(prev) prev->next = next;
    if(next) next->prev = prev;
    IMPL(A, free)(node);
    self->size -= 1;
    return data;
}

static void
IMPL(B, free)(B* self)
{
    while(self->size > 0)
    {
        T data = IMPL(B, pop)(self, self->head);
        if(self->destruct)
            self->destruct(&data);
    }
}

static void
IMPL(B, sort)(B* self, int (*compare)(const void*, const void*))
{
    T* cache = (T*) malloc(self->size * sizeof(*cache));
    int index = 0;
    A* node;
    for(node = self->head; node; node = node->next)
        cache[index++] = node->data;
    qsort(cache, self->size, sizeof(*cache), compare);
    index = 0;
    for(node = self->head; node; node = node->next)
        node->data = cache[index++];
    free(cache);
}

#undef A
#undef B
#undef T
