#ifndef T
#error "Template type T undefined for <ust.h>"
#endif

#include <ctl.h>

#define A JOIN(ust, T)
#define B JOIN(A, node)
#define I JOIN(A, it)

typedef struct B
{
    T value;
    struct B* next;
}
B;

typedef struct A
{
    void (*free)(T*);
    T (*copy)(T*);
    size_t (*hash)(T*);
    int (*equal)(T*, T*);
    B** bucket;
    size_t size;
    size_t bucket_count;
}
A;

typedef struct I
{
    void (*step)(struct I*);
    B* end;
    B* node;
    T* ref;
    B* next;
    A* container;
    size_t hash;
    int done;
}
I;

static inline int
JOIN(A, __is_prime)(size_t n)
{
    if(n == 0 || n == 1)
        return 0;
    for(size_t i = 2; i * i <= n; i++)
        if(n % i == 0)
            return 0;
    return 1;
}

static inline int
JOIN(A, __next_prime)(size_t n)
{
    while(n)
    {
        if(JOIN(A, __is_prime)(n))
            break;
        n += 1;
    }
    return n;
}

static inline B*
JOIN(B, init)(T value)
{
    B* n = malloc(sizeof(B));
    n->value = value;
    n->next = NULL;
    return n;
}

static inline B*
JOIN(B, push)(B* bucket, B* n)
{
    n->next = bucket;
    return n;
}

static inline int
JOIN(A, empty)(A* self)
{
    return self->size == 0;
}

static inline B**
JOIN(A, bucket)(A* self, T value)
{
    size_t hash = self->hash(&value) % self->bucket_count;
    return &self->bucket[hash];
}

static inline size_t
JOIN(A, bucket_size)(B* self)
{
    size_t size = 0;
    for(B* n = self; n; n = n->next)
        size += 1;
    return size;
}

static inline B*
JOIN(A, begin)(A* self)
{
    for(size_t i = 0; i < self->size; i++)
    {
        B* node = self->bucket[i];
        if(node)
            return node;
    }
    return NULL;
}

static inline B*
JOIN(A, end)(A* self)
{
    (void) self;
    return NULL;
}

static inline void
JOIN(A, free_node)(A* self, B* n)
{
    if(self->free)
        self->free(&n->value);
    free(n);
}

static inline float
JOIN(A, load_factor)(A* self)
{
    return (float) self->size / (float) self->bucket_count;
}

static inline void
JOIN(A, reserve)(A* self, size_t desired_count)
{
    self->bucket_count = JOIN(A, __next_prime)(desired_count);
    self->bucket = calloc(self->bucket_count, sizeof(B*));
}

static inline T
JOIN(A, implicit_copy)(T* self)
{
    return *self;
}

static inline A
JOIN(A, init)(size_t desired_count, size_t (*_hash)(T*), int (*_equal)(T*, T*))
{
    static A zero;
    A self = zero;
    self.hash = _hash;
    self.equal = _equal;
#ifdef P
#undef P
    self.copy = JOIN(A, implicit_copy);
#else
    self.free = JOIN(T, free);
    self.copy = JOIN(T, copy);
#endif
    JOIN(A, reserve)(&self, desired_count);
    return self;
}

static inline void
JOIN(A, insert)(A* self, T value)
{
    B** bucket = JOIN(A, bucket)(self, value);
    for(B* n = *bucket; n; n = n->next)
        if(self->equal(&value, &n->value))
        {
            if(self->free)
                self->free(&value);
            return;
        }
    *bucket = JOIN(B, push)(*bucket, JOIN(B, init)(value));
    self->size += 1;
}

static inline void
JOIN(I, step)(I* self)
{
    if(self->next == JOIN(A, end)(self->container))
    {
        for(size_t i = self->hash + 1; i < self->container->bucket_count; i++)
            if((self->next = self->container->bucket[i]))
                goto update;
        self->done = 1;
    }
    else
    {
update:
        self->node = self->next;
        self->ref = &self->node->value;
        self->next = self->node->next;
        self->hash = self->container->hash(self->ref);
    }
}

static inline I
JOIN(I, range)(A* container, B* begin, B* end)
{
    static I zero;
    I self = zero;
    if(begin)
    {
        self.step = JOIN(I, step);
        self.node = begin;
        self.ref = &self.node->value;
        self.next = self.node->next;
        self.end = end;
        self.container = container;
        self.hash = self.container->hash(self.ref);
    }
    else
        self.done = 1;
    return self;
}

static inline I
JOIN(I, each)(A* a)
{
    return JOIN(I, range)(a, JOIN(A, begin)(a), JOIN(A, end)(a));
}

static inline void
JOIN(A, rehash)(A* self, size_t desired_count)
{
    A rehashed = JOIN(A, init)(desired_count, self->hash, self->equal);
    foreach(A, self, it)
    {
        B** bucket = JOIN(A, bucket)(&rehashed, it.node->value);
        *bucket = JOIN(B, push)(*bucket, it.node);
    }
    free(self->bucket);
    *self = rehashed;
}

static inline void
JOIN(A, free)(A* self)
{
    foreach(A, self, it)
    {
        if(self->free)
            self->free(&it.node->value);
        free(it.node);
    }
    free(self->bucket);
}

static inline B*
JOIN(A, find)(A* self, T value)
{
    B** bucket = JOIN(A, bucket)(self, value);
    for(B* n = *bucket; n; n = n->next)
        if(self->equal(&value, &n->value))
            return n;
    return NULL;
}

static inline size_t
JOIN(A, count)(A* self, T value)
{
    size_t count = 0;
    foreach(A, self, it)
        if(self->equal(it.ref , &value))
            count += 1;
    return count;
}

static inline void
JOIN(A, remove)(A* self, T value)
{
    B** bucket = JOIN(A, bucket)(self, value);
    B* prev = NULL;
    for(B* n = *bucket; n; n = n->next)
    {
        if(self->equal(&value, &n->value))
        {
            B* next = n->next;
            JOIN(A, free_node)(self, n);
            if(prev)
                prev->next = next;
            self->size -= 1;
            break;
        }
        prev = n;
    }
}

static inline int
JOIN(A, equal)(A* self, A* other, int _equal(T*, T*))
{
    if(self->size != other->size)
        return 0;
    I a = JOIN(I, each)(self);
    I b = JOIN(I, each)(other);
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
JOIN(A, copy)(A* self)
{
    A other = JOIN(A, init)(self->bucket_count, self->hash, self->equal);
    foreach(A, self, it)
        JOIN(A, insert)(self, self->copy(it.ref));
    return other;
}

#undef T
#undef A
#undef B
#undef I
